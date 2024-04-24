#ifndef GRAPH_INDUCEDGRAPH
#define GRAPH_INDUCEDGRAPH

#include <vector>

#include "utils/types.h"
#include "graph/graph.h"

class InducedGraph
{
public:
    const Graph* graph;
    std::vector<uint> user_map_;
    std::vector<uint> item_map_;
    std::vector<std::pair<uint, uint>> e_lists_;

public:
    InducedGraph();
    InducedGraph(const Graph& g, uint user1, uint user2, uint item1, uint item2);
    InducedGraph(InducedGraph& g1, InducedGraph& g2, bool is_union);

    uint NumUsers() const { return user_map_.size(); }
    uint NumItems() const { return item_map_.size(); }
    uint NumEdges() const { return e_lists_.size(); }

    uint GetDegree(uint v);
};

#endif //GRAPH_INDUCEDGRAPH
