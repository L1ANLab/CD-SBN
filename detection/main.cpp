#include <chrono>
#include <iostream>
#include <numeric>
#include <string>
#include <thread>

#include "utils/CLI11.hpp"
#include "utils/globals.h"
#include "utils/types.h"

#include "graph/graph.h"
#include "detection/matching.h"
#include "detection/sj_tree.h"
#include "detection/snapshot.h"
#include "detection/continuous.h"

int main(int argc, char *argv[])
{
    CLI::App app{"App description"};

    std::string initial_graph_path = "", item_label_list_path = "", update_stream_path = "";
    int query_timestamp = -1;
    std::vector<uint> query_keywords(0);
    uint query_support_threshold = 0, query_radius = 0, query_score_threshold = 0;

    app.add_option("-i,--initial", initial_graph_path, "initial graph path")->required();
    app.add_option("-l,--labels", item_label_list_path, "initial data graph path")->required();
    app.add_option("-u,--update", update_stream_path, "update stream path")->required();
    app.add_option("-t,--qtime", query_timestamp, "query timestamp");
    app.add_option("-Q,--qkeywords", query_keywords, "query keywords (a comma separated string)")->required();
    app.add_option("-k,--qsupport", query_support_threshold, "query support threshold")->required();
    app.add_option("-r,--qradius", query_radius, "query maximum radius")->required();
    app.add_option("-s,--qscore", query_score_threshold, "query score threshold")->required();
    // app.add_option("--max-results", max_num_results, "max number of results for one edge update");
    // app.add_option("--time-limit", time_limit, "time limit for the incremental matching (second)");
    // app.add_option("--print-prep", print_prep, "print processing results or not");
    // app.add_option("--print-enum", print_enum, "print enumeration results or not");
    // app.add_option("--homo", homo, "using graph homomorphism");
    // app.add_option("--report-initial", report_initial, "report the result of initial matching or not");
    // app.add_option("--initial-time-limit", initial_time_limit, "time limit for the initial matching (second)");
    // app.add_option("--orders", orders, "pre-defined matching orders");
    
    CLI11_PARSE(app, argc, argv);
    
    std::chrono::high_resolution_clock::time_point start, lstart;

    start = Get_Time();
    std::cout << "----------- Loading graphs ------------" << std::endl;
    Graph data_graph {};
    data_graph.LoadInitialGraph(initial_graph_path);
    data_graph.LoadItemLabel(item_label_list_path);
    data_graph.PrintMetaData();
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

    /*
    std::cout << "------------ Preprocessing ------------" << std::endl;
    matching *mm = nullptr;

    start = Get_Time();

    if (algorithm == "none")
        mm                  = new matching      (query_graph, data_graph, max_num_results, print_prep, print_enum, homo);
    else
    {
        std::cout << "Unknown algorithm" << std::endl;
        exit(-1);
    }
    
    mm->Preprocessing();
    Print_Time("Preprocessing: ", start);

    if (report_initial || algorithm == "sj-tree")
    {
        std::cout << "----------- Initial Matching ----------" << std::endl;
        
        start = Get_Time();
        auto InitialFun = [&mm]()
        {
            mm->InitialMatching();
        };
        execute_with_time_limit(InitialFun, initial_time_limit, reach_time_limit);
        Print_Time("Initial Matching: ", start);
        
        size_t num_results = 0ul;
        mm->GetNumInitialResults(num_results);
        std::cout << num_results << " matches. \n";

        if (reach_time_limit) return 1;
    }

    std::cout << "--------- Incremental Matching --------" << std::endl;
    data_graph.LoadUpdateStream(stream_path);
    
    size_t num_v_updates = 0ul, num_e_updates = 0ul;

    auto IncrementalFun = [&data_graph, &mm, &num_v_updates, &num_e_updates]()
    {
        while (!data_graph.updates_.empty())
        {
            InsertUnit insert = data_graph.updates_.front();
            data_graph.updates_.pop();

            if (insert.type == 'v' && insert.is_add)
            {
                mm->AddVertex(insert.id1, insert.label);
                num_v_updates ++;
            }
            else if (insert.type == 'v' && !insert.is_add)
            {
                mm->RemoveVertex(insert.id1);
                num_v_updates ++;
            }
            else if (insert.type == 'e' && insert.is_add)
            {
                mm->AddEdge(insert.id1, insert.id2, insert.label);
                num_e_updates ++;
            }
            else if (insert.type == 'e' && !insert.is_add)
            {
                mm->RemoveEdge(insert.id1, insert.id2);
                num_e_updates ++;
            }
            if (reach_time_limit) break;
        }
    };

    start = Get_Time();

    execute_with_time_limit(IncrementalFun, time_limit, reach_time_limit);
    
    Print_Time("Incremental Matching: ", start);

    std::cout << num_v_updates << " vertex updates.\n";
    std::cout << num_e_updates << " edge updates.\n";

    size_t positive_num_results = 0ul, negative_num_results = 0ul;
    mm->GetNumPositiveResults(positive_num_results);
    mm->GetNumNegativeResults(negative_num_results);
    std::cout << positive_num_results << " positive matches.\n";
    std::cout << negative_num_results << " negative matches.\n";

    mm->PrintCounter();

    size_t num_edges = 0u, num_vertices = 0ul;
    mm->GetMemoryCost(num_edges, num_vertices);
    std::cout << "\n# edges in index in total: " << num_edges;
    std::cout << "\n# vertices in index in total: " << num_vertices;

    std::cout << "\nPeak Virtual Memory: " << mem::getValue() << " KB";
    std::cout << "\n\n----------------- End -----------------" << std::endl;

    //delete incIsoMatch;
    delete mm;
    */
    return 0;
}
