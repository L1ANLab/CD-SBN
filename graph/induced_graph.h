#ifndef GRAPH_INDUCEDGRAPH
#define GRAPH_INDUCEDGRAPH

#include <vector>

#include "utils/types.h"
#include "graph/graph.h"

class InducedGraph
{
public:
    const Graph& graph;
    std::vector<uint> user_map;
    std::vector<uint> item_map;
    std::vector<std::pair<uint, uint>> e_lists;
    std::vector<std::vector<UserData *>> user_neighbor_datas;      // The list of X and Y data for each user

public:
    InducedGraph();
    InducedGraph(const Graph& g, uint user1, uint user2, uint item1, uint item2);
    InducedGraph(InducedGraph& g1, InducedGraph& g2, bool is_union);
    InducedGraph(const Graph& g, std::vector<uint> user_map_, std::vector<uint> item_map_);
    InducedGraph(
        const Graph& g,
        std::vector<uint> user_map_,
        std::vector<uint> item_map_,
        std::vector<std::pair<uint, uint>> e_lists_
    );

    bool operator < (const InducedGraph& subgraph) const;
    bool operator == (const InducedGraph& subgraph) const;

    uint NumUsers() const { return user_map.size(); }
    uint NumItems() const { return item_map.size(); }
    uint NumEdges() const { return e_lists.size(); }
    uint GetUserDegree(uint user_id);
    
    InducedGraph* ComputeKBitruss(uint k);
    InducedGraph* ComputeSigmaBitruss(uint sigma);
    InducedGraph* ComputeKRSigmaBitruss(uint k, uint sigma, float& data_compute_time, float& edge_filter_time);
    std::string PrintMetaData();
    std::string PrintShortMetaData();
};

#endif //GRAPH_INDUCEDGRAPH
