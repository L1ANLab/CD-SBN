#ifndef GRAPH_GRAPH
#define GRAPH_GRAPH

#include <deque>
#include <tuple>
#include <vector>
#include "utils/types.h"
#include "utils/utils.h"

struct UserData
{
    uint user_id;
    uint x_data;
    uint y_data;

    UserData(): user_id(0), x_data(0), y_data(0) {}
    UserData(uint user_id_, uint x_data_, uint y_data_): user_id(user_id_), x_data(x_data_), y_data(y_data_) {}
};

struct EdgeData
{
    uint weight;
    uint ub_sup;
    EdgeData(uint weight_, uint ub_sup_): weight(weight_), ub_sup(ub_sup_) {}
    EdgeData(): weight(0), ub_sup(0) {}
};

class Graph
{
protected:
    // user
    std::vector<std::vector<uint>> user_neighbors; // Each item is a list of user neighbors
    std::vector<uint> user_bvs; // the list of support upper bound bu_sup for each user
    std::vector<uint> user_ub_sups; // the list of support upper bound bu_sup for each user
    std::vector<std::vector<UserData>> user_neighbor_datas; // the list of X and Y data for each user

    // item
    std::vector<std::vector<uint>> item_neighbors; // Each item is a list of item neighbors
    std::vector<uint> item_bvs; // the list of keyword bit vector bv for each item 
    
    // edge
    uint edge_count_;  // The number of edges
    std::vector<std::vector<uint>> edge_weights;  // edge_weights[i] is the data for edge from u_i
    
    void SetItemLabels(uint item_id, std::string label_str);

public:
    std::deque<InsertUnit> updates_;


public:
    Graph();

    virtual uint UserVerticesNum() const { return user_neighbors.size(); }
    virtual uint ItemVerticesNum() const { return item_neighbors.size(); }
    virtual uint NumEdges() const { return edge_count_; }

    void AddUserVertex(uint user_id);
    void AddItemVertex(uint item_id);
    uint InsertEdge(uint user_id, uint item_id);
    uint ExpireEdge(uint user_id, uint item_id);

    const std::vector<uint>& GetUserNeighbors(uint user_id) const;
    const std::vector<uint>& GetItemNeighbors(uint item_id) const;
    uint GetUserDegree(uint v) const;
    uint GetEdgeWeight(uint user_id, uint item_id) const;

    void LoadInitialGraph(const std::string &path);
    void LoadItemLabel(const std::string &path);
    void LoadUpdateStream(const std::string &path);
    void PrintMetaData() const;
};

#endif //GRAPH_GRAPH
