#ifndef STATISTIC_H
#define STATISTIC_H

#include <string>
#include <vector>

#include "graph/induced_graph.h"

class Statistic
{
private:
    std::string create_timestamp_folder_str;
    std::string initial_graph_path_str;
    std::string item_label_list_path_str;
    std::string update_stream_path_str;
    std::string synopsis_entries_file_path_str;
    std::string query_keywords_list_file_path_str;
    uint query_support_threshold;
    uint query_radius;
    uint query_score_threshold;
    uint query_timestamp;
    uint sliding_window_size;

public:
    Statistic(
        std::string initial_graph_path_str_,
        std::string item_label_list_path_str_,
        std::string update_stream_path_str_,
        std::string synopsis_entries_file_path_str_,
        std::string query_keywords_list_file_path_str_,
        uint query_support_threshold_,
        uint query_radius_,
        uint query_score_threshold_,
        uint query_timestamp_,
        uint sliding_window_size_
    );
    std::vector<uint> query_keywords;
    std::vector<InducedGraph*> solver_result;
    uint user_node_num;
    uint item_node_num;
    uint edge_num;
    uint all_keyword_num;

    std::chrono::high_resolution_clock::time_point start_timestamp;
    std::chrono::high_resolution_clock::time_point offline_finish_timestamp;
    std::chrono::high_resolution_clock::time_point snapshot_finish_timestamp;
    std::chrono::high_resolution_clock::time_point finish_timestamp;

    float initial_graph_load_time;
    float label_list_load_time;
    float update_stream_load_time;
    float query_keyword_load_time;
    float synopsis_building_time;

    // maintain stat
    float graph_synopsis_maintain_time;
    float edge_maintain_time;
    float graph_maintain_time;
    float synopsis_maintain_time;
    float snapshot_query_processing_time;
    float continuous_query_processing_time;

    // snapshot stat
    float select_greatest_entry_in_H_time;
    float leaf_node_traverse_time;
    float nonleaf_node_traverse_time;
    float snapshot_compute_2r_hop_time;
    float snapshot_compute_community_time;
    float snapshot_compute_data_time;
    float snapshot_filter_edge_time;

    // continuous stat
    float continuous_edge_maintain_time;
    float continuous_graph_maintain_time;
    float continuous_expired_recompute_community_time;
    float continuous_expired_compute_data_time;
    float continuous_expired_filter_edge_time;
    float continuous_expired_refine_time;
    float influenced_subgraph_compute_time;
    float continuous_inserted_compute_2r_hop_time;
    float continuous_inserted_compute_community_time;
    float continuous_inserted_compute_data_time;
    float continuous_inserted_filter_edge_time;
    float continuous_inserted_refine_time;
    float modify_result_set_time;
    float average_continuous_query_time;


    uint vertex_pruning_counter;
    uint entry_pruning_counter;
    uint leaf_node_counter;
    uint leaf_node_visit_counter;

    std::string GenerateStatisticResult();
    bool SaveStatisticResult();
    bool SaveSnapshotResult();
    bool LoadSnapshotResultExist(std::vector<InducedGraph*>& result_list, const Graph& graph);
};


#endif // STATISTIC_H