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
#include "detection/synopsis.h"


namespace fs=std::filesystem;


int main(int argc, char *argv[])
{
    CLI::App app{"App description"};

    bool is_continuous_flag = false;
    std::string initial_graph_path = "", item_label_list_path = "", update_stream_path = "";
    uint query_timestamp = 0, sliding_window_size = 0;
    std::vector<uint> query_keywords(0);
    uint query_support_threshold = 0, query_radius = 0, query_score_threshold = 0;

    app.add_flag("-c", is_continuous_flag, "whether it is a continuous query");
    app.add_option("-i,--initial", initial_graph_path, "initial graph path")->required();
    app.add_option("-l,--labels", item_label_list_path, "initial data graph path")->required();
    app.add_option("-u,--update", update_stream_path, "update stream path")->required();
    app.add_option("-t,--qtime", query_timestamp, "query timestamp")->capture_default_str();
    app.add_option("-w,--window", sliding_window_size, "the size of sliding window")->required();
    app.add_option("-Q,--qkeywords", query_keywords, "query keywords (a comma separated string)")->required();
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
        query_keywords,
        query_support_threshold,
        query_radius,
        query_score_threshold,
        query_timestamp
    );

    // std::cout << "----------- Loading graphs -----------" << std::endl;
    start = Get_Time();
    statistic->start_timestamp = start;
    // 1.1. Load item labels
    Graph* data_graph = new Graph();
    std::cout << "----------- Loading label list -----------" << std::endl;
    start = Get_Time();
    data_graph->LoadItemLabel(item_label_list_path);
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
    std::cout << "*********** Graph loading complete ***********" << std::endl;
    // data_graph->PrintMetaData();
    // Print infos
    Print_Time_Now("Load Graphs Time Cost: ", statistic->start_timestamp);
    std::cout << "* query_timestamp: " << query_timestamp << "\n";
    std::cout << "* query_keywords: " ;
    for (size_t i=0; i < query_keywords.size(); i++)
    {
        std::cout << query_keywords[i] << " ";
    }
    std::cout << "\n";
    std::cout << "* query_support_threshold: " << query_support_threshold << "\n";
    std::cout << "* query_radius: " << query_radius << "\n";
    std::cout << "* query_score_threshold: " << query_score_threshold << "\n";

    // 2. build synopsis
    Synopsis* syn = new Synopsis();
    // 2.1. precompute or load synopsis entries
    std::cout << "------------ Precompute Synopsis Entries ------------" << std::endl;
    start = Get_Time();
    std::vector<SynopsisNode*> vertex_entry_list;
    if (io::file_exists(synopsis_entries_file_path.c_str()))
    { // load from file if exists
        vertex_entry_list = syn->LoadSynopsisEntries(synopsis_entries_file_path);
        Print_Time_Now("Load part takes: ", start);
    }
    else
    { // precompute
        vertex_entry_list = syn->PrecomputeSynopsisEntries(data_graph);
        syn->SaveSynopsisEntries(synopsis_entries_file_path, vertex_entry_list);
        Print_Time_Now("Compute part takes: ", start);
    }
    // 2.2. precompute or load synopsis entries
    std::cout << "------------ Building Synopsis ------------" << std::endl;
    start = Get_Time();
    syn->BuildSynopsis(data_graph, vertex_entry_list);
    Print_Time_Now("Build part takes ", start);
    statistic->synopsis_building_time = Duration(start);

    // 2. print time cost of Building Synopsis
    Print_Time("Building Synopsis Time Cost: ", statistic->synopsis_building_time);
    std::cout << "*********** Preprocessing complete ***********" << std::endl;

    Print_Time_Now("Offline Total Time: ", statistic->start_timestamp);
    statistic->offline_finish_timestamp = Get_Time();

    // 3. execute query
    std::cout << "------------ Start online processing ------------" << std::endl;
    start = Get_Time();
    std::vector<InducedGraph*> result_list;
    if (!is_continuous_flag) // 3.1. for snapshot query
    {
        // 3.1.1. maintain the graph and synopsis until the query time
        if (query_timestamp > data_graph->GetGraphTimestamp())
        {
            std::cout << "------------ Start graph&synopsis maintenance ------------" << std::endl;
            size_t start_idx = 0, end_idx = 0;
            std::vector<InsertUnit> update_stream = data_graph->GetUpdateStream();
            ErrorControl::assert_error(
                update_stream.size() <= 0,
                "Initialization Error: The update stream has not been initialized!"
            );
            while (end_idx < update_stream.size() && update_stream[end_idx].timestamp <= query_timestamp)
            {
                // add new edge if in query
                std::cout << "Maintain timestamp " << update_stream[end_idx].timestamp << " to " << query_timestamp << std::endl;
                uint addition_flag = data_graph->InsertEdge(
                    update_stream[end_idx].user_id,
                    update_stream[end_idx].item_id
                );
                syn->UpdateSynopsisAfterInsertion(
                    update_stream[end_idx].user_id,
                    update_stream[end_idx].item_id,
                    addition_flag,
                    data_graph
                );
                // expire old edge if out of window
                if (end_idx - start_idx + 1 > sliding_window_size)
                {
                    uint removal_flag = data_graph->ExpireEdge(
                        update_stream[end_idx].user_id,
                        update_stream[end_idx].item_id
                    );
                    syn->UpdateSynopsisAfterExpiration(
                        update_stream[end_idx].user_id,
                        update_stream[end_idx].item_id,
                        removal_flag,
                        data_graph
                    );
                    start_idx += 1;
                }
                // move to next edge
                end_idx += 1;
            }
            std::cout << std::endl;
        }
        statistic->graph_synopsis_maintain_time = Duration(start);
        start = Get_Time();
        // 3.1.2. find the answer for the snapshot query
        std::cout << "------------ Start query processing ------------" << std::endl;
        SnapshotHandle* snapshot_query = new SnapshotHandle(
            query_keywords,
            query_support_threshold,
            query_radius,
            query_score_threshold,
            data_graph,
            syn
        );

        result_list = snapshot_query->ExecuteQuery(statistic);
        statistic->solver_result = result_list;
        statistic->query_process_time = Duration(start);
    }
    else // 3.2. for continuous query
    {
        statistic->start_timestamp = start;
        size_t start_idx = 0, end_idx = 0;
        std::vector<InsertUnit> update_stream = data_graph->GetUpdateStream();
        while (end_idx < update_stream.size())
        {
            // 3.2.1. maintain the graph and synopsis once
            // add new edge if in query
            uint addition_flag = data_graph->InsertEdge(
                update_stream[end_idx].user_id,
                update_stream[end_idx].item_id
            );
            syn->UpdateSynopsisAfterInsertion(
                update_stream[end_idx].user_id,
                update_stream[end_idx].item_id,
                addition_flag,
                data_graph
            );
            // expire old edge if out of window
            if (end_idx - start_idx + 1 > sliding_window_size)
            {
                uint removal_flag = data_graph->ExpireEdge(
                    update_stream[end_idx].user_id,
                    update_stream[end_idx].item_id
                );
                syn->UpdateSynopsisAfterExpiration(
                    update_stream[end_idx].user_id,
                    update_stream[end_idx].item_id,
                    removal_flag,
                    data_graph
                );
                start_idx += 1;
            }
            // TODO: 3.2.2. find the answer for the continuous query
            // SnapshotHandle* snapshot_query = new SnapshotHandle(
            //     query_keywords,
            //     query_support_threshold,
            //     query_radius,
            //     query_score_threshold,
            //     data_graph,
            //     syn
            // );

            // result_list = snapshot_query->ExecuteSnapshotQuery(statistic);
        
            // move to next edge
            end_idx += 1;
        }
        statistic->finish_timestamp = Get_Time();
    }

    statistic->finish_timestamp = Get_Time();
    std::cout << "*********** Query processing complete ***********" << std::endl;
    // print result
    // for (InducedGraph* subgraph: result_list)
    // {
    //     subgraph->PrintMetaData();
    //     std::cout << std::endl;
    // }
    statistic->user_node_num = data_graph->UserVerticesNum();
    statistic->item_node_num = data_graph->ItemVerticesNum();
    statistic->edge_num = data_graph->NumEdges();
    statistic->leaf_node_counter = syn->CountLeafNodes(syn->GetRoot());

    std::cout << std::endl << statistic->GenerateStatisticResult() << std::endl;

    if (statistic->SaveStatisticResult())
    {
        std::cout << "Print stat result successfully" << std::endl;
    }

    for (auto subgraph :result_list)
    {
        delete subgraph;
    }
    delete statistic;
    delete syn;
    delete data_graph;
    return 0;
}
