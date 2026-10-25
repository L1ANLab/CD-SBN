
#include <vector>
#include <algorithm>
#include <omp.h>
#include <stdexcept>

#include "detection/continuous_handle.h"

namespace
{
// A multi-source traversal shares overlapping searches. Unfiltered neighborhoods
// conservatively include centers whose keyword-filtered neighborhood may change.
std::vector<uint> batch_neighborhood_users(
    const Graph& graph, const std::vector<uint>& seeds, uint radius)
{
    std::set<uint> visited_users, visited_items;
    std::vector<uint> frontier;
    for (uint user : seeds)
        if (user < graph.UserVerticesNum() && visited_users.emplace(user).second)
            frontier.push_back(user);
    for (uint depth = 0; depth < radius && !frontier.empty(); ++depth)
    {
        std::vector<uint> next;
        for (uint user : frontier)
            for (uint item : graph.GetUserNeighbors(user))
                if (visited_items.emplace(item).second)
                    for (uint neighbor : graph.GetItemNeighbors(item))
                        if (visited_users.emplace(neighbor).second)
                            next.push_back(neighbor);
        frontier.swap(next);
    }
    return {visited_users.begin(), visited_users.end()};
}

void batch_union_users(std::vector<uint>& users, const std::vector<uint>& other)
{
    std::vector<uint> merged;
    merged.reserve(users.size() + other.size());
    std::set_union(users.begin(), users.end(), other.begin(), other.end(),
                   std::back_inserter(merged));
    users.swap(merged);
}

// Apply the existing user-set inclusion rule while releasing discarded graphs.
void batch_insert_community(std::set<InducedGraph*>& candidates,
                            std::unique_ptr<InducedGraph> community)
{
    if (!community || community->e_lists.empty()) return;
    for (auto iter = candidates.begin(); iter != candidates.end(); )
    {
        const auto& old_users = (*iter)->user_map;
        const auto& new_users = community->user_map;
        if (std::includes(old_users.begin(), old_users.end(), new_users.begin(), new_users.end()))
            return;
        if (std::includes(new_users.begin(), new_users.end(), old_users.begin(), old_users.end()))
        {
            delete *iter;
            iter = candidates.erase(iter);
        }
        else ++iter;
    }
    candidates.emplace(community.release());
}
} // namespace

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
    std::vector<SynopsisNode*>& vertex_entry_list,
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
        influenced_subgraph->ComputeKRSigmaBitrussSimple(
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
        // 4.1. get the vertex
        uint candidate_user_id = user_id;
        // If the vertex can be pruned, skip
        if (!CheckPruningConditions(vertex_entry_list[candidate_user_id]))
        {
            continue;
        }

        // 4.2. compute the 2r-hop of user
        inserted_compute_2r_hop_start_timestamp = Get_Time();

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
            r_hop_subgraph->ComputeKRSigmaBitrussSimple(
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

ContinuousBatchStats ContinuousHandle::ExecuteBatch(
    Statistic* stat,
    std::vector<InducedGraph*>& result_list,
    std::vector<SynopsisNode*>& vertex_entry_list,
    const std::vector<InsertUnit>& stream,
    size_t begin, size_t end, size_t& window_begin, size_t capacity)
{
    if (!stat || begin > end || end > stream.size() || window_begin > begin ||
        begin - window_begin > capacity)
        throw std::invalid_argument("ExecuteBatch: invalid stream range or window state");
    ContinuousBatchStats summary;
    if (begin == end) return summary;
    const auto batch_start = Get_Time();

    // Collect both insertions and expirations BEFORE mutating the graph. Do not
    // cancel repeated records: multiplicities affect support and relationship scores.
    auto phase_start = Get_Time();
    std::vector<uint> changed_users;
    size_t simulated_start = window_begin;
    for (size_t index = begin; index < end; ++index)
    {
        changed_users.push_back(stream[index].user_id);
        if (index - simulated_start + 1 > capacity)
            changed_users.push_back(stream[simulated_start++].user_id);
    }
    auto batch = PrepareBatch(changed_users, result_list);
    stat->influenced_subgraph_compute_time += Duration(phase_start);

    for (size_t index = begin; index < end; ++index)
    {
        const auto& added = stream[index];
        phase_start = Get_Time();
        const uint inserted = data_graph->InsertEdge(added.user_id, added.item_id);
        stat->continuous_edge_maintain_time += Duration(phase_start);

        phase_start = Get_Time();
        data_graph->MaintainAfterInsertion(added.user_id, added.item_id, inserted);
        if (added.user_id >= vertex_entry_list.size())
            vertex_entry_list.resize(static_cast<size_t>(added.user_id) + 1);
        if (!vertex_entry_list[added.user_id])
            vertex_entry_list[added.user_id] = syn->CreateVertexEntry(added.user_id, data_graph);
        stat->continuous_graph_maintain_time += Duration(phase_start);

        if (index - window_begin + 1 > capacity)
        {
            const auto& removed = stream[window_begin];
            phase_start = Get_Time();
            data_graph->MaintainBeforeExpiration(removed.user_id, removed.item_id);
            stat->continuous_graph_maintain_time += Duration(phase_start);
            phase_start = Get_Time();
            data_graph->ExpireEdge(removed.user_id, removed.item_id);
            stat->continuous_edge_maintain_time += Duration(phase_start);
            ++window_begin;
            ++summary.expirations;
        }
        ++summary.updates;
    }

    phase_start = Get_Time();
    ExecuteBatchQuery(stat, result_list, batch);
    summary.result_maintain_ms = Duration(phase_start);
    summary.candidate_centers = batch.candidate_centers.size();
    summary.recomputed_centers = batch.recomputed_centers;
    summary.invalidated_communities = batch.invalidated_communities;
    summary.total_ms = Duration(batch_start);
    return summary;
}

ContinuousHandle::BatchContext ContinuousHandle::PrepareBatch(
    const std::vector<uint>& updated_users,
    const std::vector<InducedGraph*>& result_list)
{
    BatchContext batch;
    batch.updated_users = updated_users;
    std::sort(batch.updated_users.begin(), batch.updated_users.end());
    batch.updated_users.erase(std::unique(batch.updated_users.begin(), batch.updated_users.end()),
                             batch.updated_users.end());
    batch.candidate_centers = batch_neighborhood_users(*data_graph, batch.updated_users, 2 * query_radius);
    batch.invalid_results.resize(result_list.size(), false);
    std::set<uint> invalid_users;
    for (size_t index = 0; index < result_list.size(); ++index)
        for (uint user : result_list[index]->user_map)
            if (std::binary_search(batch.candidate_centers.begin(), batch.candidate_centers.end(), user))
            {
                batch.invalid_results[index] = true;
                ++batch.invalidated_communities;
                invalid_users.insert(result_list[index]->user_map.begin(), result_list[index]->user_map.end());
                break;
            }
    // A smaller candidate previously hidden by a maximal community may become a
    // result after expiration. Include every possible old center of such candidates,
    // including centers that were themselves removed by community peeling.
    const std::vector<uint> invalid_seeds(invalid_users.begin(), invalid_users.end());
    batch_union_users(batch.candidate_centers,
                      batch_neighborhood_users(*data_graph, invalid_seeds, query_radius));
    return batch;
}

uint ContinuousHandle::ExecuteBatchQuery(
    Statistic* stat, std::vector<InducedGraph*>& result_list, BatchContext& batch)
{
    auto start = Get_Time();
    // Use both graph states: deleted paths are visible before the batch, and new
    // structures formed jointly by several insertions are visible after it.
    batch_union_users(batch.candidate_centers,
                      batch_neighborhood_users(*data_graph, batch.updated_users, 2 * query_radius));
    stat->influenced_subgraph_compute_time += Duration(start);

    std::vector<std::unique_ptr<InducedGraph>> computed(batch.candidate_centers.size());
    double hop_ms = 0, community_ms = 0;
    float data_ms = 0, filter_ms = 0;
    size_t recomputed = 0;
    const uint workers = std::max(1u, static_cast<uint>(
        std::min<size_t>(THREADS_NUM, batch.candidate_centers.size())));
#pragma omp parallel for num_threads(workers) reduction(+:hop_ms,community_ms,data_ms,filter_ms,recomputed)
    for (size_t index = 0; index < batch.candidate_centers.size(); ++index)
    {
        auto local_start = Get_Time();
        std::vector<uint> users, items;
        std::vector<std::pair<uint, uint>> edges;
        std::tie(users, items, edges) = data_graph->Get2rHopOfUserByBV(
            batch.candidate_centers[index], query_radius, query_BV);
        hop_ms += Duration(local_start);
        if (edges.empty()) continue;
        ++recomputed;
        InducedGraph neighborhood(*data_graph, std::move(users), std::move(items), std::move(edges), {});
        float local_data_ms = 0, local_filter_ms = 0;
        local_start = Get_Time();
        // Batch work deliberately avoids the unchanged single-update Synopsis
        // entries: their upper bounds need not describe this batch's final graph.
        computed[index].reset(neighborhood.ComputeKRSigmaBitrussSimple(
            query_support_threshold, query_score_threshold, local_data_ms, local_filter_ms));
        community_ms += Duration(local_start);
        data_ms += local_data_ms;
        filter_ms += local_filter_ms;
    }
    batch.recomputed_centers = recomputed;
    stat->continuous_inserted_compute_2r_hop_time += hop_ms / workers;
    stat->continuous_inserted_compute_community_time += community_ms / workers;
    stat->continuous_inserted_compute_data_time += data_ms / workers;
    stat->continuous_inserted_filter_edge_time += filter_ms / workers;

    start = Get_Time();
    std::set<InducedGraph*> candidates;
    for (size_t index = 0; index < result_list.size(); ++index)
        if (batch.invalid_results[index]) delete result_list[index];
        else candidates.emplace(result_list[index]);
    result_list.clear();
    // Stable center order makes refinement independent of OpenMP scheduling.
    for (auto& community : computed)
        batch_insert_community(candidates, std::move(community));
    result_list.assign(candidates.begin(), candidates.end());
    stat->continuous_inserted_refine_time += Duration(start);
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