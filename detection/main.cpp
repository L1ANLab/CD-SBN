#include <chrono>
#include <iostream>
#include <numeric>
#include <string>
#include <thread>

#include "utils/CLI11.hpp"
#include "utils/globals.h"
#include "utils/types.h"
#include "utils/statistic.h"

#include "graph/graph.h"
#include "detection/snapshot_handle.h"
#include "detection/continuous_handle.h"
#include "detection/synopsis.h"


int main(int argc, char *argv[])
{
    CLI::App app{"App description"};

    bool is_continuous_flag = false;
    std::string initial_graph_path = "", item_label_list_path = "", update_stream_path = "";
    uint query_timestamp = -1, sliding_window_size = 0;
    std::vector<uint> query_keywords(0);
    uint query_support_threshold = 0, query_radius = 0, query_score_threshold = 0;

    app.add_flag("-c", is_continuous_flag, "whether it is a continuous query");
    app.add_option("-i,--initial", initial_graph_path, "initial graph path")->required();
    app.add_option("-l,--labels", item_label_list_path, "initial data graph path")->required();
    app.add_option("-u,--update", update_stream_path, "update stream path")->required();
    app.add_option("-t,--qtime", query_timestamp, "query timestamp");
    app.add_option("-w,--window", sliding_window_size, "the size of sliding window")->required();
    app.add_option("-Q,--qkeywords", query_keywords, "query keywords (a comma separated string)")->required();
    app.add_option("-k,--qsupport", query_support_threshold, "query support threshold")->required();
    app.add_option("-r,--qradius", query_radius, "query maximum radius")->required();
    app.add_option("-s,--qscore", query_score_threshold, "query score threshold")->required();
    
    CLI11_PARSE(app, argc, argv);
    
    std::chrono::high_resolution_clock::time_point start;
    Statistic* statistic = new Statistic(
        initial_graph_path,
        item_label_list_path,
        update_stream_path,
        query_keywords,
        query_support_threshold,
        query_radius,
        query_score_threshold,
        query_timestamp
    );

    std::cout << "----------- Loading graphs ------------" << std::endl;
    start = Get_Time();
    // statistic->start_timestamp = start;
    // 1. Load initial graph and item labels
    Graph* data_graph = new Graph();
    data_graph->LoadInitialGraph(initial_graph_path);
    data_graph->LoadItemLabel(item_label_list_path);
    data_graph->PrintMetaData();
    // Print infos
    Print_Time("Load Graphs: ", start);
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

    std::cout << "------------ Preprocessing ------------" << std::endl;
    start = Get_Time();

    // 2. build synopsis
    Synopsis* syn = new Synopsis();

    syn->BuildSynopsis(data_graph);

    Print_Time("Synopsis Building: ", start);

    // 3. execute query
    statistic->start_timestamp = start;
    std::vector<InducedGraph*> result_list;
    if (!is_continuous_flag) // 3.1. for snapshot query
    {
        // 3.1.1. maintain the graph and synopsis until the query time
        if (query_timestamp > data_graph->GetGraphTimestamp())
        {
            size_t start_idx = 0, end_idx = 0;
            std::vector<InsertUnit> update_stream = data_graph->GetUpdateStream();
            while (query_timestamp <= update_stream[end_idx].timestamp)
            {
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
                // move to next edge
                end_idx += 1;
            }   
        }
        // 3.1.2. find the answer for the snapshot query
        SnapshotHandle* snapshot_query = new SnapshotHandle(
            query_keywords,
            query_support_threshold,
            query_radius,
            query_score_threshold,
            data_graph,
            syn
        );

        result_list = snapshot_query->ExecuteSnapshotQuery(statistic);
    }
    else // 3.2. for continuous query
    {
        size_t start_idx = 0, end_idx = 0;
        std::vector<InsertUnit> update_stream = data_graph->GetUpdateStream();
        while (start_idx < update_stream.size())
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
    }
    statistic->finish_timestamp = Get_Time();

    // TODO: print result
    for (auto subgraph :result_list)
    {
        delete subgraph;
    }
    delete statistic;
    delete syn;
    delete data_graph;
    return 0;
}
