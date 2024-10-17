#include <chrono>
#include <iostream>
#include <numeric>
#include <string>
#include <thread>
#include <filesystem>

#include "utils/CLI11.hpp"
#include "utils/globals.h"
#include "utils/types.h"
#include "utils/statistic.h"

#include "graph/graph.h"
#include "detection/snapshot_handle.h"
#include "detection/continuous_handle.h"
#include "detection/baseline_handle.h"
#include "detection/synopsis.h"


namespace fs=std::filesystem;


std::vector<std::vector<uint>> load_query_keywords_list(std::string path);

int main(int argc, char *argv[])
{
    CLI::App app{"App description"};

    bool is_baseline_flag = false;
    std::string initial_graph_path = "", item_label_list_path = "",
    update_stream_path = "", query_keywords_list_path = "";
    uint query_timestamp = 0, sliding_window_size = 0;
    uint query_support_threshold = 0, query_radius = 0, query_score_threshold = 0;

    app.add_flag("-b,--baseline", is_baseline_flag, "whether using baseline");
    app.add_option("-i,--initial", initial_graph_path, "initial graph path")->required();
    app.add_option("-l,--labels", item_label_list_path, "initial data graph path")->required();
    app.add_option("-u,--update", update_stream_path, "update stream path")->required();
    app.add_option("-t,--qtime", query_timestamp, "query timestamp")->capture_default_str();
    app.add_option("-w,--window", sliding_window_size, "the size of sliding window")->required();
    app.add_option("-q,--qkeywords", query_keywords_list_path, "query keywords list file path")->required();
    app.add_option("-k,--qsupport", query_support_threshold, "query support threshold")->required();
    app.add_option("-r,--qradius", query_radius, "query maximum radius")->required();
    app.add_option("-s,--qscore", query_score_threshold, "query score threshold")->required();
    
    CLI11_PARSE(app, argc, argv);
    
    std::chrono::high_resolution_clock::time_point start;
    fs::path initial_graph_folder = fs::path(initial_graph_path).parent_path();
    fs::path synopsis_entries_file_path = initial_graph_folder / fs::path("synopsis_entries.txt");
    Statistic* statistic = new Statistic(
        initial_graph_path,
        item_label_list_path,
        update_stream_path,
        synopsis_entries_file_path,
        query_keywords_list_path,
        query_support_threshold,
        query_radius,
        query_score_threshold,
        query_timestamp,
        sliding_window_size
    );
    // std::cout << "----------- Loading graphs -----------" << std::endl;
    start = Get_Time();
    statistic->start_timestamp = start;
    // 1.1. Load item labels
    Graph* data_graph = new Graph();
    std::cout << "----------- Loading label list -----------" << std::endl;
    start = Get_Time();
    data_graph->LoadItemLabel(item_label_list_path);
    statistic->all_keyword_num = data_graph->GetLabelSize();
    statistic->label_list_load_time = Duration(start);
    Print_Time("Load Label List Time Cost: ", statistic->label_list_load_time);
    // 1.2. Load initial graph
    std::cout << "----------- Loading initial graph -----------" << std::endl;
    data_graph->LoadInitialGraph(initial_graph_path);
    statistic->initial_graph_load_time = Duration(start);
    Print_Time("Load Initial Graph Time Cost: ", statistic->initial_graph_load_time);
    // 1.3. Load update stream
    std::cout << "----------- Loading update stream -----------" << std::endl;
    start = Get_Time();
    data_graph->LoadUpdateStream(update_stream_path);
    statistic->update_stream_load_time = Duration(start);
    Print_Time("Load Update Stream Time Cost: ", statistic->update_stream_load_time);
    // 1.4. Load query keywords from file
    std::cout << "----------- Loading query keyword -----------" << std::endl;
    start = Get_Time();
    std::vector<std::vector<uint>> query_keywords_list =
        load_query_keywords_list(query_keywords_list_path);
    statistic->query_keyword_load_time = Duration(start);
    Print_Time("Load Update Stream Time Cost: ", statistic->update_stream_load_time);
    std::cout << "*********** Graph loading complete ***********" << std::endl;
    std::cout << std::endl;

    Print_Time_Now("Load Graphs Time Cost: ", statistic->start_timestamp);
    std::cout << "* query_timestamp: " << query_timestamp << std::endl;
    std::cout << "* query_support_threshold: " << query_support_threshold << std::endl;
    std::cout << "* query_radius: " << query_radius << std::endl;
    std::cout << "* query_score_threshold: " << query_score_threshold << std::endl;
    std::cout << std::endl;


    // 2. build synopsis
    Synopsis* syn = new Synopsis();
    // 2.1. do extra computation for baseline
    if (is_baseline_flag)
    {
        // store the whole graph as a subgraph
        data_graph->ComputeTrussnessReplaceSupport();
    }

    // 2.2. precompute or load synopsis entries
    std::cout << "------------ Precompute Synopsis Entries ------------" << std::endl;
    start = Get_Time();
    std::vector<SynopsisNode*> vertex_entry_list(0);
    if (io::file_exists(synopsis_entries_file_path.c_str()))
    { // load from file if exists
        syn->LoadSynopsisEntries(synopsis_entries_file_path, vertex_entry_list);
        Print_Time_Now("Load part takes: ", start);
    }
    else
    { // precompute
        syn->PrecomputeSynopsisEntries(data_graph, vertex_entry_list);
        syn->SaveSynopsisEntries(synopsis_entries_file_path, vertex_entry_list);
        Print_Time_Now("Compute part takes: ", start);
    }
    // 2.3. precompute or load synopsis entries 
    std::cout << "------------ Building Synopsis ------------" << std::endl;
    start = Get_Time();
    syn->BuildSynopsis(data_graph, vertex_entry_list);
    Print_Time_Now("Build part takes ", start);
    statistic->synopsis_building_time = Duration(start);
    statistic->leaf_node_counter = syn->CountLeafNodes(syn->GetRoot());
    // print time cost of Building Synopsis
    Print_Time("Building Synopsis Time Cost: ", statistic->synopsis_building_time);
    std::cout << "*********** Preprocessing complete ***********" << std::endl;
    Print_Time_Now("Offline Total Time: ", statistic->start_timestamp);
    std::cout << std::endl;
    statistic->offline_finish_timestamp = Get_Time();

    // 3. maintain the graph and synopsis until the query time 
    std::cout << "------------ Start online processing ------------" << std::endl;
    std::cout << "------------ Start graph & synopsis maintenance ------------" << std::endl;
    std::chrono::high_resolution_clock::time_point edge_maintain_start, graph_maintain_start, synopsis_maintain_start;
    statistic->edge_maintain_time = 0.0;
    statistic->graph_maintain_time = 0.0;
    statistic->synopsis_maintain_time = 0.0;

    start = Get_Time();
    if (query_timestamp > data_graph->GetGraphTimestamp())
    {
        size_t start_idx = 0, end_idx = 0;
        std::vector<InsertUnit> update_stream = data_graph->GetUpdateStream();
        ErrorControl::assert_error(
            update_stream.size() <= 0,
            "Initialization Error: The update stream has not been initialized!"
        );
        while (end_idx < update_stream.size() && update_stream[end_idx].timestamp <= query_timestamp)
        {
            uint add_edge_user_id = update_stream[end_idx].user_id;
            uint add_edge_item_id = update_stream[end_idx].item_id;
            // add new edge if in quer
            // (1) insert edge
            std::cout << "Insert edge (" << add_edge_user_id << "," << add_edge_user_id << ")";
            std::cout << " at " << update_stream[start_idx].timestamp << std::endl;
            edge_maintain_start = Get_Time();
            uint addition_flag = data_graph->InsertEdge(
                add_edge_user_id,
                add_edge_item_id
            );
            statistic->edge_maintain_time += Duration(edge_maintain_start);
            Print_Time_Now("[Insert] in ",  edge_maintain_start);
            // (2) maintain grpah
            graph_maintain_start = Get_Time();
            data_graph->MaintainAfterInsertion(
                add_edge_user_id,
                add_edge_item_id,
                addition_flag
            );
            statistic->graph_maintain_time += Duration(graph_maintain_start);
            Print_Time_Now("[Maintain Graph] in ",  graph_maintain_start);
            // (3) maintain synopsis
            synopsis_maintain_start = Get_Time();
            syn->UpdateSynopsisAfterInsertion(
                add_edge_user_id,
                add_edge_item_id,
                addition_flag,
                data_graph
            );
            statistic->synopsis_maintain_time += Duration(synopsis_maintain_start);
            Print_Time_Now("[Maintain Synopsis] in ",  synopsis_maintain_start);
            // expire old edge if out of window
            if (end_idx - start_idx + 1 > sliding_window_size)
            {
                uint expire_edge_user_id = update_stream[start_idx].user_id;
                uint expire_edge_item_id = update_stream[start_idx].item_id;
                // (1) maintain grpah
                graph_maintain_start = Get_Time();
                data_graph->MaintainBeforeExpiration(
                    expire_edge_user_id,
                    expire_edge_item_id
                );
                statistic->graph_maintain_time += Duration(graph_maintain_start);
                Print_Time_Now("[Maintain Graph] in ",  graph_maintain_start);
                // (2) expire edge
                std::cout << "Expire edge (" << expire_edge_user_id << "," << expire_edge_item_id << ")";
                std::cout << " at " << update_stream[start_idx].timestamp << std::endl;
                edge_maintain_start = Get_Time();
                uint removal_flag = data_graph->ExpireEdge(
                    expire_edge_user_id,
                    expire_edge_item_id
                );
                statistic->edge_maintain_time += Duration(edge_maintain_start);
                Print_Time_Now("[Expire] in ",  edge_maintain_start);
                // (3) maintain synopsis
                synopsis_maintain_start = Get_Time();
                syn->UpdateSynopsisAfterExpiration(
                    expire_edge_user_id,
                    expire_edge_item_id,
                    removal_flag,
                    data_graph
                );
                statistic->synopsis_maintain_time += Duration(synopsis_maintain_start);
                Print_Time_Now("[Maintain Synopsis] in ",  synopsis_maintain_start);
                start_idx += 1;
            }
            // move to next edge
            end_idx += 1;
        }
        std::cout << std::endl;
    }
    statistic->graph_synopsis_maintain_time = Duration(start);
    std::cout << "*********** Graph & synopsis maintenance complete ***********" << std::endl;
    std::cout << std::endl;

    // 4. query process
    std::cout << "------------ Start snapshot query ------------" << std::endl;
    for (auto query_keywords: query_keywords_list)
    {
        statistic->query_keywords = query_keywords;
        for (size_t i=0; i < query_keywords.size(); i++)
        {
            std::cout << query_keywords[i] << " ";
        }
        std::cout << "\n";

        std::vector<InducedGraph*> result_list(0);
        // Load snapshot result from file ()
        if (!statistic->LoadSnapshotResultExist(result_list))
        {
            // 4.1. find the answer for the snapshot query
            SnapshotHandle* snapshot_query = new SnapshotHandle(
                query_keywords,
                query_support_threshold,
                query_radius,
                query_score_threshold,
                data_graph,
                syn
            );
            start = Get_Time();
            snapshot_query->ExecuteQuery(statistic, result_list);
            statistic->snapshot_query_processing_time = Duration(start);

            // statistic->user_node_num = data_graph->UserVerticesNum();
            // statistic->item_node_num = data_graph->ItemVerticesNum();
            // statistic->edge_num = data_graph->NumEdges();
            // std::cout << std::endl << statistic->GenerateStatisticResult() << std::endl;

            // if (statistic->SaveStatisticResult())
            // {
            //     std::cout << "Print stat result successfully" << std::endl;
            // }

            std::cout << "*********** Snapshot query complete ***********" << std::endl;
            std::cout << std::endl;

            std::cout << "Snapshot Result:[" << result_list.size() << "]" << std::endl;
            statistic->solver_result = result_list;
            if (statistic->SaveSnapshotResult())
            {
                std::cout << "Save snapshot result successfully" << std::endl;
            }
            
            delete snapshot_query;
        }
        else
        {
            // Load result from file

        }

        // 4.2. maintain the answer for the continuous query
        std::cout << "------------ Start continuous query ------------" << std::endl;
        ContinuousHandle* continuous_query = new ContinuousHandle(
            query_keywords,
            query_support_threshold,
            query_radius,
            query_score_threshold,
            data_graph,
            syn
        );
        std::chrono::high_resolution_clock::time_point continuous_turn_start;
        statistic->continuous_edge_maintain_time = 0.0;
        statistic->continuous_graph_maintain_time = 0.0;

        statistic->continuous_expired_recompute_community_time = 0.0;
        statistic->continuous_expired_refine_time = 0.0;
        statistic->continuous_inserted_compute_2r_hop_time = 0.0;
        statistic->continuous_inserted_compute_community_time = 0.0;
        statistic->continuous_inserted_refine_time = 0.0;

        start = Get_Time();
        size_t start_idx = 0, end_idx = 0;
        std::vector<InsertUnit> update_stream = data_graph->GetUpdateStream();
        while (end_idx < update_stream.size())
        {
            continuous_turn_start = Get_Time();
            // 4.2.1. Insertion maintanance
            uint insert_edge_user_id = UINT_MAX;
            uint insert_edge_item_id = UINT_MAX;
            std::vector<uint> insert_related_user_list(0);
            uint isInserted = 0;
            if (end_idx < update_stream.size())
            {
                insert_edge_user_id = update_stream[end_idx].user_id;
                insert_edge_item_id = update_stream[end_idx].item_id;
                // (1) insert edge
                std::cout << "Insert edge (" << insert_edge_user_id << "," << insert_edge_item_id << ")";
                std::cout << " at " << update_stream[end_idx].timestamp << std::endl;
                edge_maintain_start = Get_Time();
                isInserted = data_graph->InsertEdge(
                    insert_edge_user_id,
                    insert_edge_item_id
                );
                statistic->continuous_edge_maintain_time += Duration(edge_maintain_start);
                // Print_Time_Now("[Insert] in ",  edge_maintain_start);
                // (2) maintain grpah
                graph_maintain_start = Get_Time();
                insert_related_user_list = data_graph->MaintainAfterInsertion(
                    insert_edge_user_id,
                    insert_edge_item_id,
                    isInserted
                );
                statistic->continuous_graph_maintain_time += Duration(graph_maintain_start);
            }
            // Print_Time_Now("[Maintain] in ",  graph_maintain_start);
            // 4.2.2. Expiration maintanance
            uint expire_edge_user_id = UINT_MAX;
            uint expire_edge_item_id = UINT_MAX;
            uint isRemoved = 0;
            if (end_idx - start_idx + 1 > sliding_window_size)
            {
                expire_edge_user_id = update_stream[start_idx].user_id;
                expire_edge_item_id = update_stream[start_idx].item_id;
                std::cout << "Expire edge (" << update_stream[start_idx].user_id << "," << update_stream[start_idx].item_id << ")";
                std::cout << " at " << update_stream[start_idx].timestamp << std::endl;
                // (1) maintain grpah
                graph_maintain_start = Get_Time();
                data_graph->MaintainBVBeforeExpiration(
                    update_stream[start_idx].user_id,
                    update_stream[start_idx].item_id
                );
                statistic->continuous_graph_maintain_time += Duration(graph_maintain_start);
                // Print_Time_Now("[Maintain] in ",  graph_maintain_start);
                // (2) expire edge
                edge_maintain_start = Get_Time();
                isRemoved = data_graph->ExpireEdge(
                    update_stream[start_idx].user_id,
                    update_stream[start_idx].item_id
                );
                statistic->continuous_edge_maintain_time += Duration(edge_maintain_start);

                // 4.3. find the answer for the continuous query
                continuous_query->ExecuteQuery(
                    statistic,
                    result_list,
                    isRemoved, expire_edge_user_id, expire_edge_item_id,
                    insert_edge_user_id, insert_related_user_list
                );
                // statistic
                statistic->average_continuous_query_time = (statistic->average_continuous_query_time * (end_idx) + Duration(continuous_turn_start)) / (end_idx + 1);
                Print_Time_Now("Continuous Turn Time: ", continuous_turn_start);
                std::cout << "Continuous Result: [" << result_list.size() << "]" << " at " << update_stream[end_idx].timestamp << std::endl;
                Print_Time("Average Continuous Turn Time: ", statistic->average_continuous_query_time);
                start_idx += 1;
            }
            // move to next edge
            end_idx += 1;
        }

        statistic->solver_result = result_list;
        statistic->continuous_query_processing_time = Duration(start);

        statistic->finish_timestamp = Get_Time();
        std::cout << "*********** Continuous query complete ***********" << std::endl;
        std::cout << "*********** Query processing complete ***********" << std::endl;
        std::cout << std::endl;
        // print result
        // for (InducedGraph* subgraph: result_list)
        // {
        //     subgraph->PrintMetaData();
        //     std::cout << std::endl;
        // }
        statistic->user_node_num = data_graph->UserVerticesNum();
        statistic->item_node_num = data_graph->ItemVerticesNum();
        statistic->edge_num = data_graph->NumEdges();

        std::cout << std::endl << statistic->GenerateStatisticResult() << std::endl;

        if (statistic->SaveStatisticResult())
        {
            std::cout << "Print stat result successfully" << std::endl;
        }

        delete continuous_query;

        for (InducedGraph* result_subgraph: statistic->solver_result)
        {
            delete result_subgraph;
        }
        statistic->solver_result.clear();
    }
    
    delete statistic;
    delete syn;
    delete data_graph;
    return 0;
}

std::vector<std::vector<uint>> load_query_keywords_list(std::string path)
{
    ErrorControl::assert_error(
        !io::file_exists(path.c_str()),
        "File Error: The input <" + path  + "> file does not exists"
    );
    std::ifstream ifs(path);
    ErrorControl::assert_error(
        !ifs,
        "File Stream Error: The input file stream open failed"
    );
    uint query_keywords_num, query_keywords_size;
    ifs >> query_keywords_num >> query_keywords_size;
    std::string line_str;
    std::vector<std::vector<uint>> query_keywords_list(query_keywords_num);
    std::getline(ifs, line_str); // delete "\n"
    for (uint i=0;i<query_keywords_num;i++)
    {
        query_keywords_list[i].resize(query_keywords_size);
        std::getline(ifs, line_str);
        if (line_str.find('%') != line_str.npos) continue;
        std::stringstream ss(line_str);
        for (uint j=0;j<query_keywords_size;j++)
        {
            ss >> query_keywords_list[i][j];
        }
    }
    ifs.close();

    return query_keywords_list;
}

