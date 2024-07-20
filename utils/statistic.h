#ifndef STATISTIC_H
#define STATISTIC_H

#include <string>
#include <vector>

#include "graph/induced_graph.h"

class Statistic
{
private:
    std::string initial_graph_path_str;
    std::string item_label_list_path_str;
    std::string update_stream_path_str;
    std::string synopsis_entries_file_path_str;
    std::vector<uint> query_keywords;
    uint query_support_threshold;
    uint query_radius;
    uint query_score_threshold;
    uint query_timestamp;

public:
    Statistic(
        std::string initial_graph_path_str_,
        std::string item_label_list_path_str_,
        std::string update_stream_path_str_,
        std::string synopsis_entries_file_path_str_,
        std::vector<uint> query_keywords_,
        uint query_support_threshold_,
        uint query_radius_,
        uint query_score_threshold_,
        uint query_timestamp_
    );
    std::vector<InducedGraph*> solver_result;
    uint user_node_num;
    uint item_node_num;
    uint edge_num;

    std::chrono::high_resolution_clock::time_point start_timestamp;
    std::chrono::high_resolution_clock::time_point offline_finish_timestamp;
    std::chrono::high_resolution_clock::time_point finish_timestamp;

    float initial_graph_load_time;
    float label_list_load_time;
    float update_stream_load_time;
    float synopsis_building_time;

    float query_process_time;

    // maintain stat
    float graph_synopsis_maintain_time;
    float edge_maintain_time;
    float graph_maintain_time;
    float synopsis_maintain_time;

    // snapshot stat
    float select_greatest_entry_in_H_time;
    float leaf_node_traverse_time;
    float nonleaf_node_traverse_time;
    float snapshot_compute_2r_hop_time;
    float snapshot_compute_k_bitruss_time;
    float snapshot_compute_user_relationship_score_time;

    // continuous stat
    float continuous_expired_recompute_k_bitruss_time;
    float continuous_expired_recompute_score_time;
    float continuous_inserted_compute_2r_hop_time;
    float continuous_inserted_compute_k_bitruss_time;
    float continuous_inserted_compute_score_time;
    float modify_result_set_time;


    uint vertex_pruning_counter;
    uint entry_pruning_counter;
    uint leaf_node_counter;
    uint leaf_node_visit_counter;

    std::string GenerateStatisticResult();
    bool SaveStatisticResult(std::vector<InducedGraph*> result_list);
};


#endif // STATISTIC_H