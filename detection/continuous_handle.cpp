
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

uint ContinuousHandle::ExecuteQuery(
    Statistic* stat,
    std::vector<InducedGraph*>& result_list,
    uint isRemoved, uint expire_edge_user_id, uint expire_edge_item_id,
    uint insert_edge_user_id, std::vector<uint> insert_related_user_list
)
{
    // 0. initialization:
    std::chrono::high_resolution_clock::time_point start_timestamp,
    expired_recompute_community_start_timestamp, expired_refine_start_timestamp,
    influenced_subgraph_start_timestamp, inserted_compute_2r_hop_start_timestamp,
    inserted_compute_community_start_timestamp, inserted_refine_start_timestamp;
    float expire_community_time=0.0, expire_refine_time=0.0,
    insert_2r_hop_time=0.0, insert_community_time=0.0, insert_refine_time=0.0;

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
                bool HasRelatedItem = false;
                for (uint center_user_neighbored_item: this->data_graph->GetUserNeighbors(expire_edge_user_id))
                {
                    // check whether the expire user has another related item.
                    if ((*(this->query_BV) & *(this->data_graph->GetItemBv(center_user_neighbored_item))).any())
                    {
                        HasRelatedItem = true;
                    }
                }

                if (!HasRelatedItem)
                { // unqualified user so remove all edges
                    auto remove_iter = std::remove_if(
                        result_list[idx]->e_lists.begin(),
                        result_list[idx]->e_lists.end(),
                        [expire_edge_user_id] (std::pair<uint, uint> edge) {
                            return edge.first == expire_edge_user_id;
                        }
                    );
                    result_list[idx]->e_lists.resize(remove_iter - result_list[idx]->e_lists.begin());
                }
                else
                {
                    auto remove_iter = std::remove(
                        result_list[idx]->e_lists.begin(),
                        result_list[idx]->e_lists.end(),
                        std::pair{expire_edge_user_id, expire_edge_item_id}
                    );
                    result_list[idx]->e_lists.resize(remove_iter - result_list[idx]->e_lists.begin());
                }
            }
            expired_recompute_community_start_timestamp = Get_Time();
            // (2) compute the (k,r,σ)-bitruss
            std::unique_ptr<InducedGraph> k_r_sigma_bitruss_subgraph(
                result_list[idx]->ComputeKRSigmaBitrussSimple(
                    query_support_threshold,
                    query_score_threshold,
                    stat->continuous_expired_compute_data_time,
                    stat->continuous_expired_filter_edge_time
                )
            );
            expire_community_time += Duration(expired_recompute_community_start_timestamp);

            // (3) add the result
            expired_refine_start_timestamp = Get_Time();
            if (!k_r_sigma_bitruss_subgraph->e_lists.empty() &&
                CheckCommunityInsert(candidate_set_P, k_r_sigma_bitruss_subgraph))
                candidate_set_P.emplace(k_r_sigma_bitruss_subgraph.release());
            expire_refine_time += Duration(expired_refine_start_timestamp);
            // else delete k_r_sigma_bitruss_subgraph;
            delete result_list[idx];
        }
        else // 1.2. add subgraph to set if subgraph contains none of related user
        {
            candidate_set_P.emplace(result_list[idx]);
        }
    }
    if (candidate_set_P.size() < result_list.size())
    {
        std::cout << "Delete some subgraphs" << std::endl;
    }
    Print_Time("Expire Recompute Community: ", expire_community_time);
    // Print_Time("Expire Refine Set: ", expire_refine_time);
    stat->continuous_expired_recompute_community_time += expire_community_time;
    stat->continuous_expired_refine_time += expire_refine_time;

    // 2. get the 2r-hop of the ending user of insert edge
    influenced_subgraph_start_timestamp = Get_Time();

    std::vector<uint> influenced_user_list(0), influenced_item_list(0);
    std::vector<std::pair<uint, uint>> influenced_edge_list(0);
    if (insert_edge_user_id != UINT_MAX)
    {
        std::tie(influenced_user_list, influenced_item_list, influenced_edge_list) = data_graph->Get2rHopOfUserByBV(
            insert_edge_user_id,
            2*query_radius,
            query_BV
        );
    }
    std::cout << influenced_user_list.size() << std::endl;
    // 2.4 return if no user 
    if (influenced_user_list.size() < 2)
    {
        result_list.assign(candidate_set_P.begin(), candidate_set_P.end());
        return result_list.size();
    }

    // 3. compute the (k,r,σ)-bitruss from influenced subgraph
    inserted_compute_community_start_timestamp = Get_Time();
    std::unique_ptr<InducedGraph> influenced_subgraph(
        new InducedGraph(*data_graph, influenced_user_list, influenced_item_list, influenced_edge_list, {})
    );
    Print_Time_Now("Create New Induced Subgraph Time: ", influenced_subgraph_start_timestamp);
    std::unique_ptr<InducedGraph> influenced_k_r_sigma_bitruss_subgraph(
        influenced_subgraph->ComputeKRSigmaBitruss(
            query_support_threshold,
            query_score_threshold,
            stat->continuous_inserted_compute_data_time,
            stat->continuous_inserted_filter_edge_time
        )
    );
    stat->influenced_subgraph_compute_time += Duration(influenced_subgraph_start_timestamp);
    Print_Time_Now("Influenced Subgraph Compute: ", influenced_subgraph_start_timestamp);

    // 4. compute the 2r-hop of the rest user vertices
#pragma omp parallel for num_threads(THREADS_NUM)
    for(uint user_id: influenced_k_r_sigma_bitruss_subgraph->user_map)
    {
        inserted_compute_2r_hop_start_timestamp = Get_Time();
        // 4.1. get the vertex
        uint candidate_user_id = user_id;
        
        // 4.2. compute the 2r-hop of user
        std::vector<uint> raw_user_list, raw_item_list;
        std::vector<std::pair<uint, uint>> raw_edge_list;
        std::tie(raw_user_list, raw_item_list, raw_edge_list) = data_graph->Get2rHopOfUserByBV(
            candidate_user_id,
            query_radius,
            query_BV
        );
        std::vector<uint> user_list(
            raw_user_list.size() + influenced_k_r_sigma_bitruss_subgraph->user_map.size()
        );
        std::vector<uint>::iterator user_it = std::set_intersection(
            raw_user_list.begin(), raw_user_list.end(),
            influenced_k_r_sigma_bitruss_subgraph->user_map.begin(),
            influenced_k_r_sigma_bitruss_subgraph->user_map.end(),
            user_list.begin()
        );
        user_list.resize(user_it - user_list.begin());

        std::vector<uint> item_list(
            raw_item_list.size() + influenced_k_r_sigma_bitruss_subgraph->item_map.size()
        );
        std::vector<uint>::iterator item_it = std::set_intersection(
            raw_item_list.begin(), raw_item_list.end(),
            influenced_k_r_sigma_bitruss_subgraph->item_map.begin(),
            influenced_k_r_sigma_bitruss_subgraph->item_map.end(),
            item_list.begin()
        );
        item_list.resize(item_it - item_list.begin());

        std::vector<std::pair<uint, uint>> edge_list(
            raw_edge_list.size() + influenced_k_r_sigma_bitruss_subgraph->e_lists.size()
        );
        std::vector<std::pair<uint, uint>>::iterator edge_it = std::set_intersection(
            raw_edge_list.begin(), raw_edge_list.end(),
            influenced_k_r_sigma_bitruss_subgraph->e_lists.begin(),
            influenced_k_r_sigma_bitruss_subgraph->e_lists.end(),
            edge_list.begin()
        );
        edge_list.resize(edge_it - edge_list.begin());

        std::unique_ptr<InducedGraph> r_hop_subgraph(new InducedGraph(*data_graph, user_list, item_list, edge_list, {}));
        insert_2r_hop_time += Duration(inserted_compute_2r_hop_start_timestamp);
        // pruning the vertex if its 2r-hop does not contain the related subgraph
        if (r_hop_subgraph->e_lists.empty() ||
            !hasSameElement(r_hop_subgraph->user_map, insert_related_user_list) ||
            ((r_hop_subgraph->user_map.size() - 1) * (r_hop_subgraph->item_map.size() - 1) < this->query_support_threshold)
        )
        {
            continue;
        }
        // user_computed_counter ++;

        // 4.3. compute the (k,r,σ)-bitruss
        inserted_compute_community_start_timestamp = Get_Time();
        std::unique_ptr<InducedGraph> k_r_sigma_bitruss_subgraph(
            r_hop_subgraph->ComputeKRSigmaBitruss(
                query_support_threshold,
                query_score_threshold,
                stat->continuous_inserted_compute_data_time,
                stat->continuous_inserted_filter_edge_time
            )
        );
        #pragma omp critical
        {
            insert_community_time += Duration(inserted_compute_community_start_timestamp);

            // add subgraph into P if exists
            inserted_refine_start_timestamp = Get_Time();
            if (!k_r_sigma_bitruss_subgraph->e_lists.empty() &&
            CheckCommunityInsert(candidate_set_P, k_r_sigma_bitruss_subgraph))
                candidate_set_P.emplace(k_r_sigma_bitruss_subgraph.release());
            insert_refine_time += Duration(inserted_refine_start_timestamp);
        }
    }
    stat->continuous_inserted_compute_2r_hop_time += (insert_2r_hop_time)/THREADS_NUM;
    stat->continuous_inserted_compute_community_time += (insert_community_time)/THREADS_NUM;
    stat->continuous_inserted_refine_time += (insert_refine_time)/THREADS_NUM;
    // Print_Time("Insert Recompute 2r-hop: ", insert_2r_hop_time);
    // Print_Time("Insert Recompute Community: ", insert_community_time);
    // Print_Time("Insert Refine Set: ", insert_refine_time);
    // std::cout << "[" << user_computed_counter << "] Computed User" << std::endl;

    // 5. refine the candidate set

    result_list.assign(candidate_set_P.begin(), candidate_set_P.end());
    return result_list.size();
}


/*

uint ContinuousHandle::ExecuteQuery(
    Statistic* stat,
    std::vector<InducedGraph*>& result_list,
    uint isRemoved, uint expire_edge_user_id, uint expire_edge_item_id,
    uint insert_edge_user_id, std::vector<uint> insert_related_user_list
)
{
    // 0. initialization:
    std::chrono::high_resolution_clock::time_point start_timestamp,
    expired_recompute_community_start_timestamp, expired_refine_start_timestamp,
    influenced_subgraph_start_timestamp, inserted_compute_2r_hop_start_timestamp,
    inserted_compute_community_start_timestamp, inserted_refine_start_timestamp;
    float expire_community_time=0.0, expire_refine_time=0.0,
    insert_2r_hop_time=0.0, insert_community_time=0.0, insert_refine_time=0.0;

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
                if ((*(this->query_BV) & *(data_graph->GetUserBv(expire_edge_user_id))).none())
                { // unqualified user so remove all edges
                    auto remove_iter = std::remove_if(
                        result_list[idx]->e_lists.begin(),
                        result_list[idx]->e_lists.end(),
                        [expire_edge_user_id] (std::pair<uint, uint> edge) {
                            return edge.first == expire_edge_user_id;
                        }
                    );
                    result_list[idx]->e_lists.resize(remove_iter - result_list[idx]->e_lists.begin());
                }
                else
                {
                    auto remove_iter = std::remove(
                        result_list[idx]->e_lists.begin(),
                        result_list[idx]->e_lists.end(),
                        std::pair{expire_edge_user_id, expire_edge_item_id}
                    );
                    result_list[idx]->e_lists.resize(remove_iter - result_list[idx]->e_lists.begin());
                }
            }
            expired_recompute_community_start_timestamp = Get_Time();
            // (2) compute the (k,r,σ)-bitruss
            std::unique_ptr<InducedGraph> k_r_sigma_bitruss_subgraph(
                result_list[idx]->ComputeKRSigmaBitruss(
                    query_support_threshold,
                    query_score_threshold,
                    stat->continuous_expired_compute_data_time,
                    stat->continuous_expired_filter_edge_time
                )
            );
            expire_community_time += Duration(expired_recompute_community_start_timestamp);

            // (3) add the result
            expired_refine_start_timestamp = Get_Time();
            if (!k_r_sigma_bitruss_subgraph->e_lists.empty() &&
                CheckCommunityInsert(candidate_set_P, k_r_sigma_bitruss_subgraph))
                candidate_set_P.emplace(k_r_sigma_bitruss_subgraph.release());
            expire_refine_time += Duration(expired_refine_start_timestamp);
            // else delete k_r_sigma_bitruss_subgraph;
            delete result_list[idx];
        }
        else // 1.2. add subgraph to set if subgraph contains none of related user
        {
            candidate_set_P.emplace(result_list[idx]);
        }
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
    influenced_subgraph_start_timestamp = Get_Time();

    std::vector<uint> influenced_user_list(0), influenced_item_list(0);
    if (insert_edge_user_id != UINT_MAX)
    {
        std::tie(influenced_user_list, influenced_item_list) = data_graph->Get2rHopOfUserByBV(
            insert_edge_user_id,
            query_radius,
            query_BV
        );
    }
    std::cout << influenced_user_list.size() << std::endl;
    uint count = 0;
    for (uint influenced_user_id: influenced_user_list)
    {
        if (count == 0)
        {
            std::cout << "\r" << count+1 << "/" << influenced_user_list.size();
            std::cout << std::fixed << " ("  << (count+1)*100.0/ influenced_user_list.size() << "%)" << std::endl;
            count++;
        }
        std::vector<uint> influenced_community_user_list(0), influenced_community_item_list(0);
        std::tie(influenced_community_user_list, influenced_community_item_list) = data_graph->Get2rHopOfUserByBV(
            influenced_user_id,
            query_radius,
            query_BV
        );
        // 4. compute the 2r-hop of the rest user vertices
// #pragma omp parallel for num_threads(THREADS_NUM)
        // uint count2 = 0;
        for(uint user_id: influenced_community_user_list)
        {
            // {
            //     std::cout << "\r" << count2+1 << "/" << influenced_user_list.size();
            //     std::cout << std::fixed << " ("  << (count2+1)*100.0/ influenced_user_list.size() << "%)" << std::endl;
            //     count2++;
            // }
            inserted_compute_2r_hop_start_timestamp = Get_Time();
            // 4.1. get the vertex
            uint candidate_user_id = user_id;
            
            // 4.2. compute the 2r-hop of user
            std::vector<uint> user_list, item_list;
            std::tie(user_list, item_list) = data_graph->Get2rHopOfUserByBV(
                candidate_user_id,
                query_radius,
                query_BV
            );

            std::unique_ptr<InducedGraph> r_hop_subgraph(new InducedGraph(*data_graph, user_list, item_list));
            insert_2r_hop_time += Duration(inserted_compute_2r_hop_start_timestamp);
            // pruning the vertex if its 2r-hop does not contain the related subgraph
            if (r_hop_subgraph->e_lists.empty() ||
                !hasSameElement(r_hop_subgraph->user_map, insert_related_user_list) ||
                ((r_hop_subgraph->user_map.size() - 1) * (r_hop_subgraph->item_map.size() - 1) < this->query_support_threshold)
            )
            {
                continue;
            }
            // user_computed_counter ++;

            // 4.3. compute the (k,r,σ)-bitruss
            inserted_compute_community_start_timestamp = Get_Time();
            std::unique_ptr<InducedGraph> k_r_sigma_bitruss_subgraph(
                r_hop_subgraph->ComputeKRSigmaBitruss(
                    query_support_threshold,
                    query_score_threshold,
                    stat->continuous_inserted_compute_data_time,
                    stat->continuous_inserted_filter_edge_time
                )
            );
            // #pragma omp critical
            {
                insert_community_time += Duration(inserted_compute_community_start_timestamp);

                // add subgraph into P if exists
                inserted_refine_start_timestamp = Get_Time();
                if (!k_r_sigma_bitruss_subgraph->e_lists.empty() &&
                CheckCommunityInsert(candidate_set_P, k_r_sigma_bitruss_subgraph))
                    candidate_set_P.emplace(k_r_sigma_bitruss_subgraph.release());
                insert_refine_time += Duration(inserted_refine_start_timestamp);
            }
        }
    }
    // 4. compute the 2r-hop of the rest user vertices
    stat->continuous_inserted_compute_2r_hop_time += (insert_2r_hop_time)/THREADS_NUM;
    stat->continuous_inserted_compute_community_time += (insert_community_time)/THREADS_NUM;
    stat->continuous_inserted_refine_time += (insert_refine_time)/THREADS_NUM;
    Print_Time("Insert Recompute 2r-hop: ", insert_2r_hop_time);
    Print_Time("Insert Recompute Community: ", insert_community_time);
    Print_Time("Insert Refine Set: ", insert_refine_time);
    // std::cout << "[" << user_computed_counter << "] Computed User" << std::endl;

    // 5. refine the candidate set

    result_list.assign(candidate_set_P.begin(), candidate_set_P.end());
    return result_list.size();
}

*/