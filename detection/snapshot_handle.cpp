
#include <vector>
#include <set>
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

std::vector<InducedGraph*> SnapshotHandle::ExecuteQuery(Statistic* stat)
{
    // 0. initialization:
    std::chrono::high_resolution_clock::time_point start_timestamp,
    leaf_node_start_timestamp, compute_2r_hop_start_timestamp,
    compute_k_bitruss_start_timestamp, compute_score_start_timestamp,
    refine_candidate_set_start_timestamp;

    // 0.1 initialize a candidate set
    std::set<InducedGraph*> candidate_set_P;

    // 0.2. initialize a maximum heap
    std::vector<HeapEntry*> maximum_heap_H(0);
    SynopsisNode* root_node = this->syn->GetRoot();
    maximum_heap_H.emplace_back(
        new HeapEntry(root_node, root_node->GetUbScore(query_radius_idx))
    );

    std::make_heap(maximum_heap_H.begin(), maximum_heap_H.end(), CompareHeapEntry);

    
    // uint vertex_pruning_counter = 0;
    uint leaf_node_visit_counter = 0;
    uint entry_pruning_counter = 0;
    uint max_k_truss_cost = 0;
    uint max_score_cost = 0;

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
        if( now_heap_entry->score < this->query_score_threshold)
        {
            std::cout << "!!!!Early termination!!!!" << std::endl;
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
                    maximum_heap_H.push_back(
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
            else
            {
                entry_pruning_counter += 1;
            }
            stat->nonleaf_node_traverse_time += Duration(start_timestamp);
        }
        else
        {
            leaf_node_start_timestamp = Get_Time();
            // 1.3.2. test the pruning condition
            if (CheckPruningConditions(current_node))
            {
                if (current_node->GetUserSet().size() == 0)
                {
                    std::cout << "Meet empty synopsis node<" << current_node->GetID() << ">" << std::endl;
                }
                else
                {
                    compute_2r_hop_start_timestamp = Get_Time();
                    // (1) compute r-hop of vertex
                    uint center_user_id = current_node->GetUserSet().front();
                    std::vector<uint> user_list, item_list;
                    std::tie(user_list, item_list) = data_graph->Get2rHopOfUserByBV(
                        center_user_id,
                        query_radius_idx,
                        query_BV
                    );
                    InducedGraph* r_hop_subgraph = new InducedGraph(*data_graph, user_list, item_list);
                    stat->compute_2r_hop_time += Duration(compute_2r_hop_start_timestamp);

                    // (2) compute k-bitruss from r-hop
                    compute_k_bitruss_start_timestamp = Get_Time();
                    // compute k-bitruss function
                    InducedGraph* bitruss_subgraph = r_hop_subgraph->ComputeKBitruss(query_support_threshold);
                    // delete the r-hop subgraph
                    delete r_hop_subgraph;
                    float k_bitruss_time = Duration(compute_k_bitruss_start_timestamp);
                    stat->compute_k_bitruss_time += k_bitruss_time;
                    if (max_k_truss_cost < k_bitruss_time) max_k_truss_cost = k_bitruss_time;
                    
                    // (3) compute the (k,r,σ)-bitruss
                    compute_score_start_timestamp = Get_Time();
                    InducedGraph* k_r_sigma_bitruss_subgraph = bitruss_subgraph->ComputeKRSigmaBitruss(query_score_threshold);
                    delete bitruss_subgraph;
                    float score_time = Duration(compute_score_start_timestamp);
                    stat->compute_user_relationship_score_time += score_time;
                    if (max_score_cost < score_time) max_score_cost = score_time;
                    // (4) add subgraph into P if exists
                    if (!k_r_sigma_bitruss_subgraph->user_map.empty())
                        candidate_set_P.emplace(k_r_sigma_bitruss_subgraph);
                    else
                        delete k_r_sigma_bitruss_subgraph;
                }
            }
            stat->leaf_node_traverse_time += Duration(leaf_node_start_timestamp);
        }

        delete now_heap_entry;
    }

    std::vector<InducedGraph*> result_set_R;
    result_set_R.assign(candidate_set_P.begin(), candidate_set_P.end());
    return result_set_R;
}

