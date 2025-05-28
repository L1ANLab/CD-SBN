#ifndef GRAPH_INDUCEDGRAPH
#define GRAPH_INDUCEDGRAPH

#include <vector>

#include "utils/types.h"
#include "graph/graph.h"

struct WedgeData
{
    uint user_id;
    std::vector<uint> wedge_item_list; // element: item id

    WedgeData()
        : user_id(0), wedge_item_list(0) {}

    WedgeData(uint user_id_, std::vector<uint> wedge_item_list_)
        : user_id(user_id_), wedge_item_list(wedge_item_list_) {}
};

class InducedGraph
{
public:
    const Graph& graph;
    std::vector<uint> user_map;
    std::vector<uint> item_map;
    std::vector<std::pair<uint, uint>> e_lists;
    std::vector<std::vector<WedgeData *>> user_neighbor_datas;      // The list of X and Y data for each user

public:
    InducedGraph();
    InducedGraph(const Graph& g, uint user1, uint user2, uint item1, uint item2);
    InducedGraph(InducedGraph& g1, InducedGraph& g2, bool is_union);
    InducedGraph(
        const Graph& g,
        std::vector<uint> user_map_,
        std::vector<uint> item_map_,
        std::vector<std::pair<uint, uint>> e_lists_,
        std::vector<std::vector<WedgeData *>> user_neighbor_datas_
    );
    InducedGraph(
        const Graph& g,
        std::vector<uint> user_map_,
        std::vector<uint> item_map_,
        std::vector<std::pair<uint, uint>> e_lists_
    );

    ~InducedGraph();

    bool operator < (const InducedGraph& subgraph) const;
    bool operator == (const InducedGraph& subgraph) const;

    uint NumUsers() const { return user_map.size(); }
    uint NumItems() const { return item_map.size(); }
    uint NumEdges() const { return e_lists.size(); }
    uint GetUserDegree(uint user_id);
    uint CompareScore(uint user1, uint user2, std::vector<std::vector<uint>> user_relationship_score_list);
    uint CompareScore(uint user1, uint user2, std::vector<std::vector<std::pair<uint, uint>>>& user_relationship_score_list);
    uint CompareScore(uint user1, uint user2, std::vector<std::vector<UserData*>>& local_user_neighbor_data);
    uint FindIdxInUserDataList(uint user, uint another_user, std::vector<std::vector<UserData *>> local_user_neighbor_data);
    void RemoveItemFromUserDataList(
        uint user,
        uint item,
        std::vector<uint>& item_neighbor_list,
        std::vector<std::vector<UserData *>> local_user_neighbor_data
    );
    InducedGraph* ComputeKBitruss(uint k);
    InducedGraph* ComputeSigmaBitruss(uint sigma);
    InducedGraph* ComputeKRSigmaBitruss(uint k, uint sigma, float& data_compute_time, float& edge_filter_time);
    InducedGraph* ComputeKRSigmaBitrussSimple(uint k, uint sigma, float& data_compute_time, float& edge_filter_time);
    std::string PrintMetaData();
    std::string PrintShortMetaData();
};

#endif //GRAPH_INDUCEDGRAPH
