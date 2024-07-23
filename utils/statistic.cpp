#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cmath>
#include <filesystem>

#include "utils/types.h"
#include "utils/statistic.h"

namespace fs=std::filesystem;

Statistic::Statistic(
    std::string initial_graph_path_str_,
    std::string item_label_list_path_str_,
    std::string update_stream_path_str_,
    std::string synopsis_entries_file_path_str_,
    std::vector<uint> query_keywords_,
    uint query_support_threshold_,
    uint query_radius_,
    uint query_score_threshold_,
    uint query_timestamp_
)
: initial_graph_path_str(initial_graph_path_str_)
, item_label_list_path_str(item_label_list_path_str_)
, update_stream_path_str(update_stream_path_str_)
, synopsis_entries_file_path_str(synopsis_entries_file_path_str_)
, query_keywords(query_keywords_)
, query_support_threshold(query_support_threshold_)
, query_radius(query_radius_)
, query_score_threshold(query_score_threshold_)
, query_timestamp(query_timestamp_)
{}

std::string Statistic::GenerateStatisticResult()
{
    fs::path initial_graph_folder = fs::path(initial_graph_path_str).parent_path();

    std::string distribution = initial_graph_folder.filename().string();
    std::string file_info = initial_graph_folder.parent_path().filename().string();
    std::stringstream ss(file_info); // 将input_str转换为stringstream格式
    std::string all_keyword_num, keywords_per_vertex_num;
    std::getline(ss, all_keyword_num, '-');
    std::getline(ss, keywords_per_vertex_num, '-');

    std:: string result = "";
    result += "STATISTIC RESULT\n";
    result += "-------------FILE INFO-------------\n";
    result += ("Initial Graph File: "+ initial_graph_path_str + "\n");
    result += "Synopsis Entries File: " + synopsis_entries_file_path_str + "\n";
    result += "\n";
    result += "-------------SOLVER INFO-------------\n";
    result += "Result["+ std::to_string(solver_result.size()) +"]\n";
    result += "Total User Nodes: " + std::to_string(user_node_num) + "\n";
    result += "Total Item Nodes: " + std::to_string(item_node_num) + "\n";
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
    result += "-------------PRUNING INFO-------------\n";
    result += "Pruning Vertices: " + std::to_string(vertex_pruning_counter) + "\n";
    result += "Pruning Entries: " + std::to_string(entry_pruning_counter) + "\n";
    result += "Leaf Nodes: " + std::to_string(leaf_node_counter) + "\n";
    result += "Pruning Leaf Nodes: " + std::to_string(leaf_node_counter - leaf_node_visit_counter) + "\n";
    result += "\n";
    result += "-------------TOTAL TIME INFO-------------\n";
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
    result += "Snapshot Query Processing time: " + std::to_string(snapshot_query_processing_time) + "\n";
    result += "Continuous Query Processing time: " + std::to_string(continuous_query_processing_time) + "\n";
    result += "\n";
    result += "-------------MAINTAIN TIME INFO-------------\n";
    result += "Edge Maintain time: " + std::to_string(edge_maintain_time) + "\n";
    result += "Graph Maintain time: " + std::to_string(graph_maintain_time) + "\n";
    result += "Synopsis Maintain time: " + std::to_string(synopsis_maintain_time) + "\n";
    result += "\n";
    result += "-------------SNAPSHOT TIME INFO-------------\n";
    result += "Select Greatest Entry in Heap time: " + std::to_string(select_greatest_entry_in_H_time) + "\n";
    result += "Leaf Node Traverse time: " + std::to_string(leaf_node_traverse_time) + "\n";
    result += "NonLeaf Node Traverse time: " + std::to_string(nonleaf_node_traverse_time) + "\n";
    result += "Compute 2R-Hop time: " + std::to_string(snapshot_compute_2r_hop_time) + "\n";
    result += "Compute K-Bitruss time: " + std::to_string(snapshot_compute_k_bitruss_time) + "\n";
    result += "Compute Score time: " + std::to_string(snapshot_compute_user_relationship_score_time) + "\n";
    result += "\n";
    result += "-------------CONTINUOUS TIME INFO-------------\n";
    result += "Continuous Edge Maintain time: " + std::to_string(continuous_edge_maintain_time) + "\n";
    result += "Continuous Graph Maintain time: " + std::to_string(continuous_graph_maintain_time) + "\n";
    result += "Expiration Recompute K-Bitruss Time: " + std::to_string(continuous_expired_recompute_k_bitruss_time) + "\n";
    result += "Expiration Recompute Score Time: " + std::to_string(continuous_expired_recompute_score_time) + "\n";
    result += "Insertion Compute 2R-Hop time: " + std::to_string(continuous_inserted_compute_2r_hop_time) + "\n";
    result += "Insertion Compute K-Bitruss time: " + std::to_string(continuous_inserted_compute_k_bitruss_time) + "\n";
    result += "Insertion Compute Score time: " + std::to_string(continuous_inserted_compute_score_time) + "\n";
    result += "Refine Result Set time: " + std::to_string(modify_result_set_time) + "\n";
    return result;
}


bool Statistic::SaveStatisticResult(std::vector<InducedGraph*> result_list)
{
    fs::path initial_graph_folder = fs::path(initial_graph_path_str).parent_path();
    
    std::chrono::high_resolution_clock::time_point now_time = Get_Time();
    std::stringstream ss;
    time_t t = std::chrono::system_clock::to_time_t(now_time);
    ss << std::put_time(localtime(&t), "%m%d-%H%M%S");
    std::string time_str = ss.str() +  "-" + GetUnixTimestamp(now_time);
    std::string stat_file_name = "statistic-" + time_str + ".txt";
    std::string result_file_name = "result-" + time_str + ".txt";

    fs::path stat_file_path = initial_graph_folder / fs::path(stat_file_name);
    fs::path result_file_path = initial_graph_folder / fs::path(result_file_name);
    
    std::ofstream stat_of(stat_file_path.string(), std::ios::out);
    ErrorControl::assert_error(
        !stat_of,
        "File Stream Error: The statistic output file stream open failed"
    );
    stat_of << GenerateStatisticResult();
    stat_of.close();

    std::ofstream result_of(result_file_path.string(), std::ios::out);
    ErrorControl::assert_error(
        !result_of,
        "File Stream Error: The result output file stream open failed"
    );
    for (auto result_subgraph: result_list)
    {
        result_of << result_subgraph->PrintMetaData() << '\n';
    }
    result_of.close();

    std::cout << "Result is saved in " << stat_file_path.string() << std::endl;
    return true;
}
