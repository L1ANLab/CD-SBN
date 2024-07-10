#include <vector>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <filesystem>

#include "utils/types.h"
#include "utils/statistic.h"

namespace fs=std::filesystem;

Statistic::Statistic(
    std::string initial_graph_path_str_,
    std::string item_label_list_path_str_,
    std::string update_stream_path_str_,
    std::vector<uint> query_keywords_,
    uint query_support_threshold_,
    uint query_radius_,
    uint query_score_threshold_,
    uint query_timestamp_
)
: initial_graph_path_str(initial_graph_path_str_)
, item_label_list_path_str(item_label_list_path_str_)
, update_stream_path_str{update_stream_path_str_}
, query_keywords(query_keywords_)
, query_support_threshold(query_support_threshold_)
, query_radius(query_radius_)
, query_score_threshold(query_score_threshold_)
, query_timestamp(query_timestamp_)
{}

std::string Statistic::GenerateStatisticResult()
{
    fs::path initial_graph_folder = fs::path(initial_graph_path_str).parent_path();
    
    fs::path mid_file_path = initial_graph_folder / fs::path("mid_graph.txt");

    std::string distribution = initial_graph_folder.filename().string();
    std::string file_info = initial_graph_folder.parent_path().filename().string();
    std::stringstream ss(file_info); // 将input_str转换为stringstream格式
    std::string all_keyword_num, keywords_per_vertex_num;
    std::getline(ss, all_keyword_num, '-');
    std::getline(ss, keywords_per_vertex_num, '-');

    std:: string result = "";
    result += "STATISTIC RESULT\n";
    result += "-------------FILE INFO-------------\n";
    result += ("Initial Graph File:"+ initial_graph_path_str + "\n");
    result += "Mid Data File: " + mid_file_path.string() + "\n";
    result += "\n";
    result += "-------------SOLVER INFO-------------\n";
    // result += "Result: {}\n".format([result[1] for result in self.solver_result]);
    result += "Total Nodes: " + std::to_string(node_num) + "\n";
    result += "Total Edges: " + std::to_string(edge_num) + "\n";
    result += "All Keywords: " + all_keyword_num + "\n";
    result += "Keywords Per Vertex: "+ keywords_per_vertex_num +"\n";
    result += "Distribution: " + distribution + "\n";
    result += "\n";
    result += "-------------QUERY INFO-------------\n";
    std::string query_keywords_str = "";
    for (uint keyword: query_keywords)
    {
        query_keywords_str += (std::to_string(keyword) + " ");
    }
    result += "Query Keywords: " + query_keywords_str + "\n";
    result += "Query Support Threshold: " + std::to_string(query_support_threshold) + "\n";
    result += "Query Radius: " + std::to_string(query_radius) + "\n";
    result += "Query Score Threshold: " + std::to_string(query_score_threshold) + "\n";
    result += "\n";
    result += "-------------Pruning INFO-------------\n";
    result += "Pruning Vertices: " + std::to_string(vertex_pruning_counter) + "\n";
    result += "Pruning Entries: " + std::to_string(entry_pruning_counter) + "\n";
    result += "Leaf Nodes: " + std::to_string(leaf_node_counter) + "\n";
    result += "Pruning Leaf Nodes: " + std::to_string(leaf_node_counter - leaf_node_visit_counter) + "\n";
    result += "\n";
    result += "-------------TIME INFO-------------\n";
    // result += "Started at: " + std::to_string(start_timestamp) + " \tFinished at: " + std::to_string(finish_timestamp) + "\n";
    float total_time = Duration(start_timestamp) - Duration(finish_timestamp);
    result += "Total time: " + std::to_string(total_time) + "\n";
    float total_offline_time = Duration(start_timestamp) - Duration(offline_finish_timestamp);
    result += "Total Offline time: " + std::to_string(total_offline_time) + "\n";
    result += "Initial Graph Load time: " + std::to_string(initial_graph_load_time) + "\n";
    result += "Label List Load time: " + std::to_string(label_list_load_time) + "\n";
    result += "Update Stream Load time: " + std::to_string(update_stream_load_time) + "\n";
    result += "Synopsis Building time: " + std::to_string(synopsis_building_time) + "\n";
    float total_online_time = Duration(offline_finish_timestamp) - Duration(finish_timestamp);
    result += "Total Online time: " + std::to_string(total_online_time) + "\n";
    result += "Maintenance time: " + std::to_string(graph_synopsis_maintain_time) + "\n";
    result += "Processing time: " + std::to_string(query_process_time) + "\n";
    result += "-------------OBTAIN TIME INFO-------------\n";
    result += "Select Greatest Entry in Heap time: " + std::to_string(select_greatest_entry_in_H_time) + "\n";
    result += "Leaf Node Traverse time: " + std::to_string(leaf_node_traverse_time) + "\n";
    result += "NonLeaf Node Traverse time: " + std::to_string(nonleaf_node_traverse_time) + "\n";
    result += "Compute 2R-Hop time: " + std::to_string(compute_2r_hop_time) + "\n";
    result += "Compute K-Bitruss time: " + std::to_string(compute_k_bitruss_time) + "\n";
    result += "Compute Relationship Score time: " + std::to_string(compute_user_relationship_score_time) + "\n";
    result += "Modify Result Set time: " + std::to_string(modify_result_set_time) + "\n";
    result += "-------------REFINE TIME INFO-------------\n";
    result += "Select Greatest Increment Entry in Heap time: " + std::to_string(select_greatest_increment_entry_time) + "\n";
    result += "Refinement Increment Compute Time: " + std::to_string(refinement_increment_compute_time);
    result += " for " + std::to_string(refinement_increment_compute_counter) + " times\n";
    result += "Refinement Graph Update Time: " + std::to_string(refinement_graph_update_time) + "\n";
    result += "Refinement Graph Copy Time: " + std::to_string(refinement_grah_copy_time) + "\n";
    return result;
}