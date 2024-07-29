#ifndef GRAPH_GRAPH
#define GRAPH_GRAPH

#include <deque>
#include <tuple>
#include <vector>
#include <bitset>
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

struct UserDataComparer
{
    bool operator() (const UserData* u1, const UserData* u2)
    {
        return u1->user_id < u2->user_id;
    }
};

class Graph
{
protected:
    // time
    uint graph_timestamp;
    // user
    std::vector<std::vector<uint>> user_neighbors; // Each item is a list of user neighbors
    std::vector<std::bitset<MAX_LABEL>> user_bvs; // the list of support upper bound bu_sup for each user
    std::vector<uint> user_ub_sups; // the list of support upper bound ub_sup for each user
    std::vector<std::vector<UserData*>> user_neighbor_datas; // the list of X and Y data for each user

    // item
    uint label_size;
    std::vector<std::vector<uint>> item_neighbors; // Each item is a list of item neighbors
    std::vector<std::bitset<MAX_LABEL>> item_bvs; // the list of keyword bit vector bv for each item 
    
    // edge
    uint edge_count_;  // The number of edges
    std::vector<std::vector<EdgeData*>> edges_;  // edges_[i] is the data for edge from u_i
    
    void SetItemLabels(uint item_id, std::string label_str);

public:
    std::vector<InsertUnit> updates_;


public:
    Graph();
    ~Graph();

    uint GetGraphTimestamp() const;
    void SetGraphTimestamp(uint new_timestamp);
    uint UserVerticesNum() const { return user_neighbors.size(); }
    uint ItemVerticesNum() const { return item_neighbors.size(); }
    uint NumEdges() const { return edge_count_; }

    void AddUserVertex(uint user_id);
    void AddItemVertex(uint item_id);
    uint InsertEdge(uint user_id, uint item_id);
    std::vector<uint> MaintainAfterInsertion(uint user_id, uint item_id, uint type);
    uint ExpireEdge(uint user_id, uint item_id);
    void MaintainBeforeExpiration(uint user_id, uint item_id);
    void MaintainBVBeforeExpiration(uint user_id, uint item_id);

    const std::vector<uint>& GetUserNeighbors(uint user_id) const;
    const std::vector<uint>& GetItemNeighbors(uint item_id) const;
    uint GetUserDegree(uint v) const;
    const std::bitset<MAX_LABEL>& GetUserBv(uint user_id) const;
    const std::bitset<MAX_LABEL>& GetItemBv(uint item_id) const;
    EdgeData* GetEdgeData(uint user_id, uint item_id) const;
    const std::vector<UserData*>& GetNeighborUserData(uint user_id) const;
    UserData* GetNeighborUserData(uint user_id, uint n_user_id) const;
    size_t InsertNeighborUserData(uint user_id, uint n_user_id);
    std::tuple<std::vector<uint>, std::vector<uint>> Get2rHopOfUser(uint center_user_id, uint r);
    std::tuple<std::vector<uint>, std::vector<uint>> Get2rHopOfUserByBV(
        uint center_user_id,
        uint r,
        std::bitset<MAX_LABEL> bv
    );
    std::vector<InsertUnit> GetUpdateStream() const;

    void LoadInitialGraph(const std::string &path);
    void LoadItemLabel(const std::string &path);
    void LoadUpdateStream(const std::string &path);
    void PrintMetaData() const;
};

#endif //GRAPH_GRAPH
