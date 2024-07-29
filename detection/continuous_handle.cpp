
#include <vector>
#include <algorithm>
#include <omp.h>

#include "detection/continuous_handle.h"

ContinuousHandle::ContinuousHandle(
    std::vector<uint> query_keywords_,
    uint query_support_threshold_,
    uint query_radius_,
    uint query_score_threshold_,
    Graph* data_graph_,
    Synopsis* syn_
)
: QueryHandle(
    query_keywords_,
    query_support_threshold_,
    query_radius_,
    query_score_threshold_,
    data_graph_,
    syn_
) {}

ContinuousHandle::~ContinuousHandle() {}


bool hasSameElement(std::vector<uint> vec1, std::vector<uint> vec2)
{
    for (uint val: vec1)
        if (std::binary_search(vec2.begin(), vec2.end(), val))
            return true;
    return false;
}

std::vector<InducedGraph*> ContinuousHandle::ExecuteQuery(
    Statistic* stat,
    std::vector<InducedGraph*> result_list,
    uint isRemoved, uint expire_edge_user_id, uint expire_edge_item_id,
    uint insert_edge_user_id, std::vector<uint> insert_related_user_list
)
{
    // 0. initialization:
    std::chrono::high_resolution_clock::time_point start_timestamp,
    expired_recompute_community_start_timestamp, expired_refine_start_timestamp,
    inserted_compute_2r_hop_start_timestamp, inserted_compute_community_start_timestamp,
    inserted_refine_start_timestamp;
    float expire_community_time=0, expire_refine_time=0,
    insert_2r_hop_time=0, insert_community_time=0, insert_refine_time=0;

    // 0.1. initialize a candidate set
    std::set<InducedGraph*> candidate_set_P;

    // 1. process the expired edge if exists
    // recompute (k ,r, \sigma)-bitruss if subgraph contains a the expired edge
    for (size_t idx = 0; idx < result_list.size(); idx++)
    {
        if (std::binary_search(
            result_list[idx]->e_lists.begin(),
            result_list[idx]->e_lists.end(),
            std::pair{expire_edge_user_id, expire_edge_item_id}
        ))
        {
            // (1) recompute k-bitruss if edge is removed
            if (isRemoved > 0)
            {
                std::vector<std::pair<uint, uint>>::iterator remove_iter;
                if ((this->query_BV & result_list[idx]->graph.GetUserBv(expire_edge_user_id)).none())
                { // unqualified user so remove all edges
                    remove_iter = std::remove_if(
                        result_list[idx]->e_lists.begin(),
                        result_list[idx]->e_lists.end(),
                        [expire_edge_user_id] (std::pair<uint, uint> edge) {
                            return edge.first == expire_edge_user_id;
                        }
                    );
                }
                else
                {
                    remove_iter = std::remove(
                        result_list[idx]->e_lists.begin(),
                        result_list[idx]->e_lists.end(),
                        std::pair{expire_edge_user_id, expire_edge_item_id}
                    );
                }
                result_list[idx]->e_lists.resize(remove_iter - result_list[idx]->e_lists.begin());
            }
            expired_recompute_community_start_timestamp = Get_Time();
            // (2) compute the (k,r,σ)-bitruss
            InducedGraph* k_r_sigma_bitruss_subgraph =
                result_list[idx]->ComputeKRSigmaBitruss(
                    query_support_threshold,
                    query_score_threshold,
                    stat->continuous_expired_compute_data_time,
                    stat->continuous_expired_filter_edge_time
                );
            expire_community_time += Duration(expired_recompute_community_start_timestamp);

            // (3) add the result
            expired_refine_start_timestamp = Get_Time();
            if (!k_r_sigma_bitruss_subgraph->e_lists.empty() &&
                CheckCommunityInsert(candidate_set_P, k_r_sigma_bitruss_subgraph))
                candidate_set_P.emplace(k_r_sigma_bitruss_subgraph);
            else delete k_r_sigma_bitruss_subgraph;
            expire_refine_time += Duration(expired_refine_start_timestamp);
        }
        else // 1.2. add subgraph to set if subgraph contains none of related user
            candidate_set_P.emplace(result_list[idx]);
    }
    if (candidate_set_P.size() < result_list.size())
    {
        std::cout << "Delete some subgraphs" << std::endl;
    }
    Print_Time("Expire Recompute Community: ", expire_community_time);
    Print_Time("Expire Refine Set: ", expire_refine_time);
    stat->continuous_expired_recompute_community_time += expire_community_time;
    stat->continuous_expired_refine_time += expire_refine_time;

    // 2. get the 2r-hop of the ending user of insert edge
    std::tuple<std::vector<uint>, std::vector<uint>> result_tuple = data_graph->Get2rHopOfUserByBV(
        insert_edge_user_id,
        query_radius,
        query_BV
    );
    std::vector<uint> insert_user_2r_hop_user_list = std::get<0>(result_tuple);
    std::cout << insert_user_2r_hop_user_list.size() << std::endl;
    // uint user_computed_counter = 0;
    #pragma omp parallel for num_threads(THREADS_NUM)
    for(uint user_id: insert_user_2r_hop_user_list)
    {
        inserted_compute_2r_hop_start_timestamp = Get_Time();
        // 2.1. get the vertex
        uint center_user_id = user_id;
        // pruning the vertex if its synopsis node is unqualified
        std::vector<SynopsisNode*> synopsis_node_list = this->syn->GetInvListByUser(center_user_id);
        bool isSkip = false;
        for (auto node: synopsis_node_list)
        {
            if (!CheckPruningConditions(node))
            {
                isSkip = true;
                break;
            }
        }
        if (isSkip) continue;
        
        // 2.2. compute the 2r-hop of user
        std::vector<uint> user_list, item_list;
        std::tie(user_list, item_list) = data_graph->Get2rHopOfUserByBV(
            center_user_id,
            query_radius,
            query_BV
        );
        InducedGraph* r_hop_subgraph = new InducedGraph(*data_graph, user_list, item_list);
        insert_2r_hop_time += Duration(inserted_compute_2r_hop_start_timestamp);
        // pruning the vertex if its 2r-hop does not contain the related subgraph
        if (r_hop_subgraph->e_lists.empty() || !hasSameElement(r_hop_subgraph->user_map, insert_related_user_list))
        {
            delete r_hop_subgraph;
            continue;
        }
        // user_computed_counter ++;

        // 2.3. compute the (k,r,σ)-bitruss
        inserted_compute_community_start_timestamp = Get_Time();
        InducedGraph* k_r_sigma_bitruss_subgraph =
            r_hop_subgraph->ComputeKRSigmaBitruss(
                query_support_threshold,
                query_score_threshold,
                stat->continuous_inserted_compute_data_time,
                stat->continuous_inserted_filter_edge_time
            );
        delete r_hop_subgraph;
        #pragma omp critical
        {
            insert_community_time += Duration(inserted_compute_community_start_timestamp);

            // (3) add subgraph into P if exists
            inserted_refine_start_timestamp = Get_Time();
            if (!k_r_sigma_bitruss_subgraph->e_lists.empty() &&
            CheckCommunityInsert(candidate_set_P, k_r_sigma_bitruss_subgraph))
                candidate_set_P.emplace(k_r_sigma_bitruss_subgraph);
            else delete k_r_sigma_bitruss_subgraph;
            insert_refine_time += Duration(inserted_refine_start_timestamp);
        }
    }
    stat->continuous_inserted_compute_2r_hop_time += (insert_2r_hop_time)/THREADS_NUM;
    stat->continuous_inserted_compute_community_time += (insert_community_time)/THREADS_NUM;
    stat->continuous_inserted_refine_time += (insert_refine_time)/THREADS_NUM;
    Print_Time("Insert Recompute 2r-hop: ", insert_2r_hop_time);
    Print_Time("Insert Recompute Community: ", insert_community_time);
    Print_Time("Insert Refine Set: ", insert_refine_time);
    // std::cout << "[" << user_computed_counter << "] Computed User" << std::endl;

    // 3. refine the candidate set
    std::vector<InducedGraph*> result_set_R;
    result_set_R.assign(candidate_set_P.begin(), candidate_set_P.end());
    return result_set_R;
}

