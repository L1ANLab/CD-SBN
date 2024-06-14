#include <string>
#include <vector>

class Statistic
{
private:
    std::string initial_graph_path_str;
    std::string item_label_list_path_str;
    std::string update_stream_path_str;
    std::vector<uint> query_keywords;
    uint query_support_threshold;
    uint query_radius;
    uint query_score_threshold;

public:
    Statistic(
        std::string initial_graph_path_str_,
        std::string item_label_list_path_str_,
        std::string update_stream_path_str_,
        std::vector<uint> query_keywords_,
        uint query_support_threshold_,
        uint query_radius_,
        uint query_score_threshold_
    );
    uint node_num;
    uint edge_num;

    std::chrono::high_resolution_clock::time_point start_timestamp;
    std::chrono::high_resolution_clock::time_point finish_timestamp;
    float select_greatest_entry_in_H_time;
    float leaf_node_traverse_time;
    float nonleaf_node_traverse_time;
    float compute_r_hop_time;
    float compute_k_truss_time;
    float compute_influential_score_time;
    float modify_result_set_time;
    float obtainment_time;
    float refinement_time;

    float select_greatest_increment_entry_time;
    float refinement_increment_compute_time;
    float refinement_increment_compute_counter;
    float refinement_graph_update_time;
    float refinement_grah_copy_time;

    float vertex_pruning_counter;
    float entry_pruning_counter;
    float leaf_node_counter;
    float leaf_node_visit_counter;

    std::string GenerateStatisticResult();
};