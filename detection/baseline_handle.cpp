
#include <vector>
#include <algorithm>

#include "detection/baseline_handle.h"

BaselineHandle::BaselineHandle(
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

BaselineHandle::~BaselineHandle() {}

std::vector<InducedGraph*> BaselineHandle::ExecuteQuery(Statistic* stat)
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

    stat->vertex_pruning_counter = (this->data_graph->UserVerticesNum() - vertex_pruning_counter);
    stat->entry_pruning_counter = entry_pruning_counter;
    stat->leaf_node_visit_counter = leaf_node_visit_counter;

    std::vector<InducedGraph*> result_set_R;
    result_set_R.assign(candidate_set_P.begin(), candidate_set_P.end());
    return result_set_R;
}

