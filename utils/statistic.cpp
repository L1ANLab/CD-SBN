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
    std::string query_keywords_list_file_path_str_,
    uint query_support_threshold_,
    uint query_radius_,
    uint query_score_threshold_,
    uint query_timestamp_,
    uint sliding_window_size_
)
: initial_graph_path_str(initial_graph_path_str_)
, item_label_list_path_str(item_label_list_path_str_)
, update_stream_path_str(update_stream_path_str_)
, synopsis_entries_file_path_str(synopsis_entries_file_path_str_)
, query_keywords_list_file_path_str(query_keywords_list_file_path_str_)
, query_support_threshold(query_support_threshold_)
, query_radius(query_radius_)
, query_score_threshold(query_score_threshold_)
, query_timestamp(query_timestamp_)
, sliding_window_size(sliding_window_size_)
{
    // create_timestamp_folder_str
    std::chrono::high_resolution_clock::time_point now_time = Get_Time();
    std::stringstream ss;
    time_t t = std::chrono::system_clock::to_time_t(now_time);
    ss << std::put_time(localtime(&t), "%m%d-%H%M%S");
    std::string time_str = ss.str() +  "-" + GetUnixTimestamp(now_time);
    this->create_timestamp_folder_str = "experiment-" + time_str;
    fs::path initial_graph_folder = fs::path(initial_graph_path_str_).parent_path();
    fs::create_directory(initial_graph_folder / fs::path(create_timestamp_folder_str));


    std::vector<uint> query_keywords(0);
    std::vector<std::unique_ptr<InducedGraph>> solver_result(0);
    this->user_node_num = 0;
    this->item_node_num = 0;
    this->edge_num = 0;
    this->all_keyword_num = 0;

    this->start_timestamp = Get_Time();
    this->offline_finish_timestamp = Get_Time();
    this->snapshot_finish_timestamp = Get_Time();
    this->finish_timestamp = Get_Time();

    this->initial_graph_load_time = 0.0;
    this->label_list_load_time = 0.0;
    this->update_stream_load_time = 0.0;
    this->query_keyword_load_time = 0.0;
    this->synopsis_building_time = 0.0;

    // maintain stat
    this->graph_synopsis_maintain_time = 0.0;
    this->edge_maintain_time = 0.0;
    this->graph_maintain_time = 0.0;
    this->synopsis_maintain_time = 0.0;
    this->snapshot_query_processing_time = 0.0;
    this->continuous_query_processing_time = 0.0;

    // snapshot stat
    this->select_greatest_entry_in_H_time = 0.0;
    this->leaf_node_traverse_time = 0.0;
    this->nonleaf_node_traverse_time = 0.0;
    this->snapshot_compute_2r_hop_time = 0.0;
    this->snapshot_compute_community_time = 0.0;
    this->snapshot_compute_data_time = 0.0;
    this->snapshot_filter_edge_time = 0.0;

    // continuous stat
    this->continuous_edge_maintain_time = 0.0;
    this->continuous_graph_maintain_time = 0.0;
    this->continuous_expired_recompute_community_time = 0.0;
    this->continuous_expired_compute_data_time = 0.0;
    this->continuous_expired_filter_edge_time = 0.0;
    this->continuous_expired_refine_time = 0.0;
    this->influenced_subgraph_compute_time = 0.0;
    this->continuous_inserted_compute_2r_hop_time = 0.0;
    this->continuous_inserted_compute_community_time = 0.0;
    this->continuous_inserted_compute_data_time = 0.0;
    this->continuous_inserted_filter_edge_time = 0.0;
    this->continuous_inserted_refine_time = 0.0;
    this->modify_result_set_time = 0.0;
    this->average_continuous_query_time = 0.0;


    this->vertex_pruning_counter = 0;
    this->entry_pruning_counter = 0;
    this->leaf_node_counter = 0;
    this->leaf_node_visit_counter = 0;
}

std::string Statistic::GenerateStatisticResult()
{
    fs::path initial_graph_folder = fs::path(initial_graph_path_str).parent_path();

    std::string distribution = initial_graph_folder.filename().string();
    // std::string file_info = initial_graph_folder.parent_path().filename().string();
    // std::stringstream ss(file_info);
    std::stringstream ss(query_keywords_list_file_path_str);
    std::string query_keywords_size;
    std::getline(ss, query_keywords_size, '-');

    std:: string result = "";
    result += "STATISTIC RESULT\n";
    result += "-------------FILE INFO-------------\n";
    result += ("Initial Graph File: "+ initial_graph_path_str + "\n");
    result += "Query Keywords File: " + query_keywords_list_file_path_str + "\n";
    result += "\n";
    result += "-------------SOLVER INFO-------------\n";
    result += "Result["+ std::to_string(solver_result.size()) +"]\n";
    result += "Total User Nodes: " + std::to_string(user_node_num) + "\n";
    result += "Total Item Nodes: " + std::to_string(item_node_num) + "\n";
    result += "Total Edges: " + std::to_string(edge_num) + "\n";
    result += "All Keywords: " + std::to_string(all_keyword_num) + "\n";
    result += "Distribution: " + distribution + "\n";
    result += "Thread Num: " + std::to_string(THREADS_NUM) + "\n";
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
    result += "Sliding Window Size: " + std::to_string(sliding_window_size) + "\n";
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
    result += "Query Keywords List Load time: " + std::to_string(query_keyword_load_time) + "\n";
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
    result += "Compute Community time: " + std::to_string(snapshot_compute_community_time) + "\n";
    result += "Compute Data time: " + std::to_string(snapshot_compute_data_time) + "\n";
    result += "Filter Edge time: " + std::to_string(snapshot_filter_edge_time) + "\n";
    result += "\n";
    result += "-------------CONTINUOUS TIME INFO-------------\n";
    result += "Continuous Edge Maintain time: " + std::to_string(continuous_edge_maintain_time) + "\n";
    result += "Continuous Graph Maintain time: " + std::to_string(continuous_graph_maintain_time) + "\n";
    result += "Expiration Recompute Community Time: " + std::to_string(continuous_expired_recompute_community_time) + "\n";
    result += "Expiration Compute Data time: " + std::to_string(continuous_expired_compute_data_time) + "\n";
    result += "Expiration Filter Edge time: " + std::to_string(continuous_expired_filter_edge_time) + "\n";
    result += "Influenced Subgraph time: " + std::to_string(influenced_subgraph_compute_time) + "\n";
    result += "Insertion Compute 2R-Hop time: " + std::to_string(continuous_inserted_compute_2r_hop_time) + "\n";
    result += "Insertion Compute Community time: " + std::to_string(continuous_inserted_compute_community_time) + "\n";
    result += "Insertion Compute Data time: " + std::to_string(continuous_inserted_compute_data_time) + "\n";
    result += "Insertion Filter Edge time: " + std::to_string(continuous_inserted_filter_edge_time) + "\n";
    result += "Refine Result Set time: " + std::to_string(modify_result_set_time) + "\n";
    result += "Average Query Process Time: " + std::to_string(average_continuous_query_time) + "\n";
    return result;
}

bool Statistic::SaveStatisticResult()
{
    fs::path initial_graph_folder = fs::path(initial_graph_path_str).parent_path();
    
    std::chrono::high_resolution_clock::time_point now_time = Get_Time();
    std::stringstream ss;
    time_t t = std::chrono::system_clock::to_time_t(now_time);
    ss << std::put_time(localtime(&t), "%m%d-%H%M%S");
    std::string time_str = ss.str() +  "-" + GetUnixTimestamp(now_time);
    std::string stat_file_name = "statistic-" + time_str + ".txt";
    std::string result_file_name = "result-" + time_str + ".txt";

    fs::path stat_file_path = initial_graph_folder / fs::path(create_timestamp_folder_str) /fs::path(stat_file_name);
    fs::path result_file_path = initial_graph_folder/ fs::path(create_timestamp_folder_str)  / fs::path(result_file_name);
    
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
    for (uint result_idx = 0; result_idx < this->solver_result.size(); result_idx++)
    {
        result_of << solver_result[result_idx]->PrintMetaData() << '\n';
    }
    result_of.close();

    std::cout << "Result is saved in " << stat_file_path.string() << std::endl;
    return true;
}


bool Statistic::SaveSnapshotResult()
{
    fs::path initial_graph_folder = fs::path(initial_graph_path_str).parent_path();

    std::string query_keywords_str = "";
    for (uint i=0; i<query_keywords.size(); i++)
    {
        query_keywords_str += (std::to_string(query_keywords[i]));
        if (i < query_keywords.size() - 1)
            query_keywords_str += "-";
    }
    std::string snapshot_result_file_name = query_keywords_str + ".txt";
    std::string snapshot_result_folder_name = "snapshot_result";
    snapshot_result_folder_name += "-s" + std::to_string(sliding_window_size);
    snapshot_result_folder_name += "-k" + std::to_string(query_support_threshold);
    snapshot_result_folder_name += "-r" + std::to_string(query_radius);
    snapshot_result_folder_name += "-S" + std::to_string(query_score_threshold);
    snapshot_result_folder_name += "-Q" + std::to_string(query_keywords.size());
    fs::path snapshot_result_folder = fs::path(snapshot_result_folder_name);

    // Create folder
    if (!fs::exists(initial_graph_folder / snapshot_result_folder))
        fs::create_directory(initial_graph_folder / snapshot_result_folder);

    // Save result
    fs::path result_file_path = initial_graph_folder/ snapshot_result_folder / fs::path(snapshot_result_file_name);
    std::ofstream result_of(result_file_path.string(), std::ios::out);
    ErrorControl::assert_error(
        !result_of,
        "File Stream Error: The result output file stream open failed"
    );
    for (uint result_idx = 0; result_idx < this->solver_result.size(); result_idx++)
    {
        result_of << solver_result[result_idx]->PrintMetaData() << '\n';
    }
    result_of.close();

    std::cout << "Result is saved in " << result_file_path.string() << std::endl;
    return true;
}

bool Statistic::LoadSnapshotResultExist(std::vector<InducedGraph*>& result_list, const Graph& graph)
{
    fs::path initial_graph_folder = fs::path(initial_graph_path_str).parent_path();
    // Construct query keyword string
    std::string query_keywords_str = "";
    for (uint i=0; i<query_keywords.size(); i++)
    {
        query_keywords_str += (std::to_string(query_keywords[i]));
        if (i < query_keywords.size() - 1)
            query_keywords_str += "-";
    }
    std::string snapshot_result_file_name = query_keywords_str + ".txt";
    // Load file by FS
    std::string snapshot_result_folder_name = "snapshot_result";
    snapshot_result_folder_name += "-s" + std::to_string(sliding_window_size);
    snapshot_result_folder_name += "-k" + std::to_string(query_support_threshold);
    snapshot_result_folder_name += "-r" + std::to_string(query_radius);
    snapshot_result_folder_name += "-S" + std::to_string(query_score_threshold);
    snapshot_result_folder_name += "-Q" + std::to_string(query_keywords.size());
    fs::path snapshot_result_folder = fs::path(snapshot_result_folder_name);
    fs::path result_file_path = initial_graph_folder / snapshot_result_folder / fs::path(snapshot_result_file_name);
    if (fs::exists(result_file_path))
    {
        ErrorControl::assert_error(
            !io::file_exists(result_file_path.string().c_str()),
            "File Error: The input <" + result_file_path.string()  + "> file does not exists"
        );
        std::ifstream ifs(result_file_path.string());
        ErrorControl::assert_error(
            !ifs,
            "File Stream Error: The input file stream open failed"
        );
        std::string line_str;
        while (std::getline(ifs, line_str))
        {
            if (line_str.size() == 0) continue;
            std::string header1, header2, user_id_str, item_id_str, edge_str, edge_user_id_str, edge_item_id_str;
            std::vector<uint> user_list(0), item_list(0);
            std::vector<std::pair<uint, uint>> edge_list(0);

            // For user list
            std::stringstream ss(line_str);
            ss >> header1 >> header2;
            while (std::getline(ss, user_id_str, ' '))
            {
                if (user_id_str.size() == 0) continue;
                std::stringstream trans(user_id_str);
                uint user_id = 0;
                trans >> user_id;
                user_list.emplace_back(user_id);
            }
            // For item list
            ss.str("");
            ss.clear();
            std::getline(ifs, line_str);
            ss << line_str;
            ss >> header1 >> header2;
            while (std::getline(ss, item_id_str, ' '))
            {
                if (item_id_str.size() == 0) continue;
                std::stringstream trans(item_id_str);
                uint item_id;
                trans >> item_id;
                item_list.emplace_back(item_id);
            }
            // For edge list
            ss.str("");
            ss.clear();
            std::getline(ifs, line_str);
            ss << line_str;
            ss >> header1 >> header2;
            // Split the line into multiple edge strings (user_id, item_id)
            while (std::getline(ss, edge_str, ' '))
            {
                if (edge_str.size() == 0) continue;
                // Extract the id pair of each edge string
                std::stringstream pair_split(edge_str.substr(1, edge_str.size()-2));
                // Get user_id
                std::getline(pair_split, edge_user_id_str, ',');
                std::stringstream trans1(edge_user_id_str);
                uint edge_user_id;
                trans1 >> edge_user_id;
                // Get item_id
                std::getline(pair_split, edge_item_id_str, ',');
                std::stringstream trans2(edge_item_id_str);
                uint edge_item_id;
                trans2 >> edge_item_id;
                edge_list.emplace_back(std::pair(edge_user_id, edge_item_id));
            }
            ss.str("");

            result_list.push_back(
                new InducedGraph(
                    graph,
                    user_list,
                    item_list,
                    edge_list
                )
            );

        }
        ifs.close();
        return true;
    }
    return false;
}