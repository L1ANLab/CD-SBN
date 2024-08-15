
#include <vector>
#include <algorithm>

#include "detection/snapshot_handle.h"

SnapshotHandle::SnapshotHandle(
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

SnapshotHandle::~SnapshotHandle() {}

uint SnapshotHandle::ExecuteQuery(Statistic* stat, std::vector<InducedGraph*>& result_list)
{
    // 0. initialization:
    std::chrono::high_resolution_clock::time_point start_timestamp,
    leaf_node_start_timestamp, compute_2r_hop_start_timestamp,
    compute_community_start_timestamp, refine_candidate_set_start_timestamp;

    // 0.1 initialize a candidate set
    std::set<InducedGraph*> candidate_set_P;

    // 0.2. initialize a maximum heap
    std::vector<HeapEntry*> maximum_heap_H(0);
    SynopsisNode* root_node = this->syn->GetRoot();
    maximum_heap_H.emplace_back(
        new HeapEntry(root_node, root_node->GetUbScore(query_radius_idx))
    );

    std::make_heap(maximum_heap_H.begin(), maximum_heap_H.end(), CompareHeapEntry);

    
    uint vertex_pruning_counter = 0;
    uint leaf_node_visit_counter = 0;
    uint entry_pruning_counter = 0;
    uint max_score_cost = 0;

    stat->select_greatest_entry_in_H_time = 0.0;
    stat->nonleaf_node_traverse_time = 0.0;
    stat->snapshot_compute_2r_hop_time = 0.0;
    stat->leaf_node_traverse_time = 0.0;
    stat->snapshot_compute_community_time = 0.0;
    stat->leaf_node_traverse_time = 0.0;

    // 1. traverse the index
    while(maximum_heap_H.size() > 0)
    {
        start_timestamp = Get_Time();
        // 1.1. get the maximum entry at the top of maximum heap. 
        std::pop_heap(maximum_heap_H.begin(), maximum_heap_H.end(), CompareHeapEntry);
        HeapEntry* now_heap_entry = maximum_heap_H.back();
        maximum_heap_H.pop_back();
        stat->select_greatest_entry_in_H_time += (Duration(start_timestamp));

        // 1.2. early exit if the score of now entry is lower than the query threshold
        if(now_heap_entry->score < this->query_score_threshold)
        {
            std::cout << "!!!!Early termination!!!!" << std::endl;
            delete now_heap_entry;
            break;
        }
        SynopsisNode* current_node = now_heap_entry->node;
        // 1.3. process the current synopsis node
        if (current_node->GetLevel() < MAX_LEVEL)
        {
            start_timestamp = Get_Time();
            // 1.3.1. test the pruning condition
            if (CheckPruningConditions(current_node))
            {
                // add the children into maximum heap if passing
                for (SynopsisNode* child_node: current_node->GetChildren())
                {
                    maximum_heap_H.emplace_back(
                        new HeapEntry(
                            child_node,
                            child_node->GetUbScore(query_radius_idx)
                        )
                    );
                    std::push_heap(maximum_heap_H.begin(), maximum_heap_H.end(), CompareHeapEntry);
                }
                // count the number of leaf index node
                if (current_node->GetChildren().empty()
                || current_node->GetChildren().back()->GetLevel() == MAX_LEVEL)
                {
                    leaf_node_visit_counter += 1;
                }
            }
            else entry_pruning_counter += 1;
            stat->nonleaf_node_traverse_time += Duration(start_timestamp);
        }
        else
        {
            leaf_node_start_timestamp = Get_Time();
            // 1.3.2. test the pruning condition
            if (CheckPruningConditions(current_node))
            {
                if (current_node->GetUserSet().size() > 0)
                {
                    compute_2r_hop_start_timestamp = Get_Time();
                    // (1) compute r-hop of vertex
                    uint center_user_id = current_node->GetUserSet().front();
                    std::vector<uint> user_list, item_list;
                    std::tie(user_list, item_list) = data_graph->Get2rHopOfUserByBV(
                        center_user_id,
                        query_radius,
                        query_BV
                    );
                    std::unique_ptr<InducedGraph> r_hop_subgraph(new InducedGraph(*data_graph, user_list, item_list));
                    stat->snapshot_compute_2r_hop_time += Duration(compute_2r_hop_start_timestamp);
                    if (r_hop_subgraph->e_lists.empty())
                    {
                        stat->leaf_node_traverse_time += Duration(leaf_node_start_timestamp);
                        delete now_heap_entry;
                        continue;
                    }
                    vertex_pruning_counter += 1;

                    // (2) compute the (k,r,σ)-bitruss
                    compute_community_start_timestamp = Get_Time();
                    std::unique_ptr<InducedGraph> k_r_sigma_bitruss_subgraph(r_hop_subgraph->ComputeKRSigmaBitruss(
                        query_support_threshold,
                        query_score_threshold,
                        stat->snapshot_compute_data_time,
                        stat->snapshot_filter_edge_time
                    ));
                    float score_time = Duration(compute_community_start_timestamp);
                    stat->snapshot_compute_community_time += score_time;
                    if (max_score_cost < score_time) max_score_cost = score_time;

                    if (k_r_sigma_bitruss_subgraph->user_map.size() > 1)
                    {
                        std::cout << "(" << center_user_id << ") ";
                        std::cout << k_r_sigma_bitruss_subgraph->PrintShortMetaData() << "\n"; 
                    }

                    // (4) add subgraph into P if exists
                    if (!k_r_sigma_bitruss_subgraph->e_lists.empty() &&
                        CheckCommunityInsert(candidate_set_P, k_r_sigma_bitruss_subgraph))
                        candidate_set_P.emplace(k_r_sigma_bitruss_subgraph.release());
                }
            }
            stat->leaf_node_traverse_time += Duration(leaf_node_start_timestamp);
        }
        delete now_heap_entry;
    }
    
    while (!maximum_heap_H.empty())
    {
        HeapEntry* now_heap_entry = maximum_heap_H.back();
        maximum_heap_H.pop_back();
        delete now_heap_entry;
    }
    

    stat->vertex_pruning_counter = (this->data_graph->UserVerticesNum() - vertex_pruning_counter);
    stat->entry_pruning_counter = entry_pruning_counter;
    stat->leaf_node_visit_counter = leaf_node_visit_counter;

    result_list.assign(candidate_set_P.begin(), candidate_set_P.end());
    return result_list.size();
}

