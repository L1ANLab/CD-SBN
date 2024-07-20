#include <algorithm>
#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>
#include <tuple>
#include <vector>
#include "utils/types.h"
#include "utils/utils.h"
#include "graph/graph.h"
#include "induced_graph.h"
#include <set>

Graph::Graph()
: graph_timestamp(0)
, user_neighbors{}
, user_bvs{}
, user_ub_sups{}
, user_neighbor_datas{}
, label_size(0)
, item_neighbors{}
, item_bvs{}
, edge_count_(0)
, edges_{}
, updates_{}
{}

Graph::~Graph()
{
    for(size_t i=0;i<user_neighbor_datas.size();i++)
    {
        for (size_t j=0;j<user_neighbor_datas[i].size();j++)
        {
            delete user_neighbor_datas[i][j];
        }
        std::vector<UserData*>().swap(user_neighbor_datas[i]);
    }
    std::vector<std::vector<UserData*>>().swap(user_neighbor_datas);

    for(size_t i=0;i<edges_.size();i++)
    {
        for (size_t j=0;j<edges_[i].size();j++)
        {
            delete edges_[i][j];
        }
        std::vector<EdgeData*>().swap(edges_[i]);
    }
    std::vector<std::vector<EdgeData*>>().swap(edges_);
}

uint Graph::GetGraphTimestamp() const { return graph_timestamp; }

void Graph::SetGraphTimestamp(uint new_timestamp) { this->graph_timestamp = new_timestamp; }

void Graph::SetItemLabels(uint item_id, std::string label_str)
{
    std::bitset<MAX_LABEL> bitvector = 0;
    std::stringstream ss(label_str);
    std::stringstream trans;
    std::string label;
    while (std::getline(ss, label, ','))
    {
        std::stringstream trans(label);
        uint label_num;
        trans >> label_num;
        bitvector.set(label_num);
    }
    AddItemVertex(item_id);
    item_bvs[item_id] = bitvector;
}


/// @brief add a new user vertex into graph
/// @param user_id 
void Graph::AddUserVertex(uint user_id)
{
    if (user_id >= user_neighbors.size())
    {
        user_neighbors.resize(user_id + 1);
        edges_.resize(user_id + 1);
        user_bvs.resize(user_id + 1);
        user_ub_sups.resize(user_id + 1);
        user_neighbor_datas.resize(user_id + 1);
    }
}

/// @brief add a new item vertex into graph
/// @param item_id 
void Graph::AddItemVertex(uint item_id)
{
    if (item_id >= item_neighbors.size())
    {
        item_neighbors.resize(item_id + 1);
        item_bvs.resize(item_id + 1);
    }
}


/// @brief maintain auxiliary data (BV, ub_sup, X, Y) after edge insertion
/// @param user_id 
/// @param item_id 
/// @param addition_flag 1 if new edge was added, else 0
/// @return return a list of user whose properties changed
std::vector<uint> Graph::MaintainAfterInsertion(uint user_id, uint item_id, uint addition_flag)
{
    EdgeData* inserted_edge = GetEdgeData(user_id, item_id);
    std::set<uint> related_user_set;
    related_user_set.emplace(user_id);

    // 1. re-compute user relationship score
    for(size_t i = 0;i < item_neighbors[item_id].size();i++)
    {   
        uint n_user_id = item_neighbors[item_id][i];
        if (n_user_id == user_id) continue;  // skip the user of <user_id>
        // 1.1. make sure new user neighbor has data
        size_t n_user_data_index = InsertNeighborUserData(user_id, n_user_id);
        size_t user_data_index = InsertNeighborUserData(n_user_id, user_id);
        uint wedge_score = GetEdgeData(n_user_id, item_id)->weight;
        if (inserted_edge->weight <= wedge_score)
        {
            uint lambda = 1;
            wedge_score = inserted_edge->weight - 1;

            // 1.2. apply the increment
            user_neighbor_datas[user_id][n_user_data_index]->x_data += lambda;
            user_neighbor_datas[user_id][n_user_data_index]->y_data += (2*lambda*wedge_score+lambda*lambda);
        
            user_neighbor_datas[n_user_id][user_data_index]->x_data += lambda;
            user_neighbor_datas[n_user_id][user_data_index]->y_data += (2*lambda*wedge_score+lambda*lambda);
            related_user_set.emplace(n_user_id);
        }
    }

    // 2. if new edge was added (addition_flag == 1)
    if (addition_flag == 1)
    {
        // 2.1. add item.BV to user.BV
        ErrorControl::assert_error(
            item_bvs.size() <= item_id,
            "Item Entity Error: No such item BV."
        );
        user_bvs[user_id] |= item_bvs[item_id];
        
        // 2.2. re-compute the support 
        for(size_t i = 0;i < item_neighbors[item_id].size();i++)
        {   
            uint n_user_id = item_neighbors[item_id][i];
            if (n_user_id == user_id) continue;  // skip the user of <user_id>
            std::vector<uint> common_neighbors(
                user_neighbors[user_id].size() + user_neighbors[n_user_id].size()
            );
            std::vector<uint>::iterator it = std::set_intersection(
                user_neighbors[user_id].begin(), user_neighbors[user_id].end(),
                user_neighbors[n_user_id].begin(), user_neighbors[n_user_id].end(),
                common_neighbors.begin()
            );
            common_neighbors.resize(it - common_neighbors.begin());
            size_t cn_num = common_neighbors.size() - 1;  // skip the item of <item_id>
            if (cn_num > 0)
            {
                inserted_edge->ub_sup += uint(cn_num);
                GetEdgeData(n_user_id, item_id)->ub_sup += uint(cn_num);
                for(size_t j = 0; j < common_neighbors.size(); j++)
                {
                    uint cn_item_id = common_neighbors[j];
                    if (cn_item_id == item_id) continue;
                    GetEdgeData(user_id, cn_item_id)->ub_sup += 1;
                    GetEdgeData(n_user_id, cn_item_id)->ub_sup += 1;
                }
                related_user_set.emplace(n_user_id);
            }
        }
    }

    std::vector<uint> related_user;
    related_user.assign(related_user_set.begin(), related_user_set.end());
    return related_user;
}

/// @brief insert an edge into graph
/// @param user_id 
/// @param item_id 
/// @return return 1 if new edge was added, else 0
uint Graph::InsertEdge(uint user_id, uint item_id)
{
    // 1. resize the neighbor list and edge weight list
    AddUserVertex(user_id);
    AddItemVertex(item_id);
    // 2. search for a place to insert neighbor
    auto lower = std::lower_bound(user_neighbors[user_id].begin(), user_neighbors[user_id].end(), item_id);
    size_t dis = std::distance(user_neighbors[user_id].begin(), lower);
    // 3.1 if inserting an existing edge, weight++ and return
    if (lower != user_neighbors[user_id].end() && *lower == item_id)
    {
        edges_[user_id][dis]->weight =  edges_[user_id][dis]->weight + 1;
        return 0;
    }
    // 3.2 else add edge into the user neighbors and item neighbors
    user_neighbors[user_id].insert(lower, item_id);
    edges_[user_id].insert(edges_[user_id].begin() + dis, new EdgeData(1, 0));

    lower = std::lower_bound(item_neighbors[item_id].begin(), item_neighbors[item_id].end(), user_id);
    item_neighbors[item_id].insert(lower, user_id);
    edge_count_++;
    return 1;
}

/// @brief maintain auxiliary data (BV, ub_sup, X, Y) after edge expiration
/// @param user_id 
/// @param item_id 
/// @param removal_flag 1 if new edge was removed, else 0
/// @return return a list of user whose properties changed
std::vector<uint> Graph::MaintainAfterExpiration(uint user_id, uint item_id, uint removal_flag)
{
    EdgeData* inserted_edge = GetEdgeData(user_id, item_id);
    std::set<uint> related_user_set;
    related_user_set.emplace(user_id);

    // 1. re-compute user relationship score
    for(size_t i = 0;i < item_neighbors[item_id].size();i++)
    {   
        uint n_user_id = item_neighbors[item_id][i];
        if (n_user_id == user_id) continue;  // delete the user of <user_id>
        // 1.1. make sure new user neighbor has data
        size_t n_user_data_index = InsertNeighborUserData(user_id, n_user_id);
        size_t user_data_index = InsertNeighborUserData(n_user_id, user_id);
        uint wedge_score = GetEdgeData(n_user_id, item_id)->weight;
        if (inserted_edge->weight < wedge_score)
        {
            uint lambda = -1;
            wedge_score = inserted_edge->weight + 1;

            // 1.2. apply the increment
            user_neighbor_datas[user_id][n_user_data_index]->x_data += lambda;
            user_neighbor_datas[user_id][n_user_data_index]->y_data += (2*lambda*wedge_score+lambda*lambda);

            user_neighbor_datas[n_user_id][user_data_index]->x_data += lambda;
            user_neighbor_datas[n_user_id][user_data_index]->y_data += (2*lambda*wedge_score+lambda*lambda);
            related_user_set.emplace(n_user_id);
        }
    }

    // 2. if new edge was added (removal_flag == 1)
    if (removal_flag == 1)
    {
        // 2.1. add item.BV to user.BV
        ErrorControl::assert_error(
            item_bvs.size() <= item_id,
            "Item Entity Error: No such item BV."
        );
        user_bvs[user_id] |= item_bvs[item_id];
        
        // 2.2. re-compute the support 
        for(size_t i = 0;i < item_neighbors[item_id].size();i++)
        {   
            uint n_user_id = item_neighbors[item_id][i];
            if (n_user_id == user_id) continue;  // delete the user of <user_id>
            std::vector<uint> common_neighbors(
                user_neighbors[user_id].size() + user_neighbors[n_user_id].size()
            );
            std::vector<uint>::iterator it = std::set_intersection(
                user_neighbors[user_id].begin(), user_neighbors[user_id].end(),
                user_neighbors[n_user_id].begin(), user_neighbors[n_user_id].end(),
                common_neighbors.begin()
            );
            common_neighbors.resize(it - common_neighbors.begin());
            size_t cn_num = common_neighbors.size() - 1;  // delete the item of <item_id>
            if (cn_num > 0)
            {
                inserted_edge->ub_sup += uint(cn_num);
                GetEdgeData(n_user_id, item_id)->ub_sup += uint(cn_num);
                for(size_t j = 0; j < common_neighbors.size(); j++)
                {
                    uint cn_item_id = common_neighbors[j];
                    if (cn_item_id == item_id) continue;
                    GetEdgeData(user_id, cn_item_id)->ub_sup += 1;
                    GetEdgeData(n_user_id, cn_item_id)->ub_sup += 1;
                }
                related_user_set.emplace(n_user_id);
            }
        }
    }

    std::vector<uint> related_user;
    related_user.assign(related_user_set.begin(), related_user_set.end());
    return related_user;
}

/// @brief expire an edge from graph
/// @param user_id 
/// @param item_id 
/// @return return 1 if new edge was removed, else 0
uint Graph::ExpireEdge(uint user_id, uint item_id)
{
    // 1. change the weight of edge
    // 1.1. search the pos of the item in the neighbor list of the user
    auto lower = std::lower_bound(user_neighbors[user_id].begin(), user_neighbors[user_id].end(), item_id);
    ErrorControl::assert_error(
        lower == user_neighbors[user_id].end() || *lower != item_id,
        "Edge Deletion Error: The edge does not exist in user_neighbors!"
    );
    // 1.2. weight-- if weight > 1
    size_t dis = std::distance(user_neighbors[user_id].begin(), lower);
    edges_[user_id][dis]->weight = edges_[user_id][dis]->weight - 1;
    // edges_[user_id].erase(edges_[user_id].begin() + );
    if (edges_[user_id][dis]->weight > 0)
    {
        return 0;
    }
    ErrorControl::assert_error(
        edges_[user_id][dis]->weight < 0,
        "Edge Deletion Error: The edge to be deleted with weight 0!"
    );
    // 2. remove the edge if weight == 1
    // 2.1. delete the item from user neighbor list
    user_neighbors[user_id].erase(lower);
    
    // 2.2 delete the user from item neighbor list
    lower = std::lower_bound(item_neighbors[item_id].begin(), item_neighbors[item_id].end(), user_id);
    ErrorControl::assert_error(
        lower == item_neighbors[item_id].end() || *lower != user_id,
        "Edge Deletion Error: The edge does not exist in item_neighbors!"
    );
    item_neighbors[item_id].erase(lower);

    // 2.2 delete the user from item neighbor list
    edge_count_--;
    return 1;
}


const std::vector<uint>& Graph::GetUserNeighbors(uint user_id) const
{
    return user_neighbors[user_id];
}

const std::vector<uint>& Graph::GetItemNeighbors(uint item_id) const
{
    return item_neighbors[item_id];
}

uint Graph::GetUserDegree(uint user_id) const
{
    return user_neighbors[user_id].size();
}

const std::bitset<MAX_LABEL>& Graph::GetUserBv(uint user_id) const
{
    return user_bvs[user_id];
}
const std::bitset<MAX_LABEL>& Graph::GetItemBv(uint item_id) const
{
    return item_bvs[item_id];
}

/// @brief get the edge data of edge[user_id][item_id]
/// @param user_id 
/// @param item_id 
/// @return return a EdgeData of edge[user_id][item_id]
EdgeData* Graph::GetEdgeData(uint user_id, uint item_id) const
{
    const std::vector<uint> *nbrs = &GetUserNeighbors(user_id);
    const std::vector<EdgeData*> *user_edge_weights = &edges_[user_id];
    uint other = item_id;
    
    long start = 0, end = nbrs->size() - 1, mid;
    while (start <= end)
    {
        mid = (start + end) / 2;
        if (nbrs->at(mid) < other)
        {
            start = mid + 1;
        }
        else if (nbrs->at(mid) > other)
        {
            end = mid - 1;
        }
        else
        {
            return user_edge_weights->at(mid);
        }
    }
    return nullptr;
}

/// @brief get user relationship score data of neighbors
/// @param user_id 
/// @return a vector of user relationship score data of user_id and each neighbor
const std::vector<UserData*>& Graph::GetNeighborUserData(uint user_id) const
{
    return user_neighbor_datas[user_id];
}
/// @brief get a user relationship score data of user_id and n_user_id
/// @param user_id 
/// @param item_id 
/// @return return a user relationship score data of user_id and n_user_id
UserData* Graph::GetNeighborUserData(uint user_id, uint n_user_id) const
{
    const std::vector<UserData*> nbrs = user_neighbor_datas[user_id];
    uint other = n_user_id;
    
    long start = 0, end = nbrs.size() - 1, mid;
    while (start <= end)
    {
        mid = (start + end) / 2;
        if (nbrs.at(mid)->user_id < other)
        {
            start = mid + 1;
        }
        else if (nbrs.at(mid)->user_id  > other)
        {
            end = mid - 1;
        }
        else
        {
            return nbrs.at(mid);
        }
    }
    return nullptr;
}

/// @brief insert an UserData into user_neighbor_datas
/// @param user_id 
/// @param n_user_id 
/// @return the index of inserted user data
size_t Graph::InsertNeighborUserData(uint user_id, uint n_user_id)
{
    const UserData* other = new UserData(n_user_id, 0, 0);
    auto lower = std::lower_bound(
        user_neighbor_datas[user_id].begin(),
        user_neighbor_datas[user_id].end(),
        other,
        [] (const UserData* u1, const UserData* u2)
        {
            return u1->user_id < u2->user_id;
        }
    );
    delete other;
    // insert new user data if not
    if (lower == user_neighbor_datas[user_id].end() || (*lower)->user_id != n_user_id)
    {
        lower = user_neighbor_datas[user_id].insert(lower, new UserData(n_user_id, 0, 0));
    }
    // uint temp = user_neighbor_datas[user_id][0]->user_id;
    // return the index of inserted user data
    size_t index = std::distance(user_neighbor_datas[user_id].begin(), lower);
    return index;
}

std::tuple<std::vector<uint>, std::vector<uint>>  Graph::Get2rHopOfUser(uint center_user_id, uint r)
{
    std::queue<uint> to_visit_users;
    std::set<uint> visited_users;
    std::queue<uint> to_visit_items;
    std::set<uint> visited_items;

    to_visit_users.emplace(center_user_id);
    for (uint i=0;i < r; i++)
    {
        while (!to_visit_users.empty())
        {
            uint visit_user = to_visit_users.front();
            to_visit_users.pop();
            if (visited_users.find(visit_user) != visited_users.end()) continue;
            for (size_t j = 0; j < user_neighbors[visit_user].size(); j++)
            {
                to_visit_items.push(user_neighbors[visit_user][j]);
            }
            visited_users.emplace(visit_user);
        }

        while (!to_visit_items.empty())
        {
            uint visit_item = to_visit_items.front();
            to_visit_items.pop();
            if (visited_items.find(visit_item) != visited_items.end()) continue;
            
            for (size_t j = 0; j < item_neighbors[visit_item].size(); j++)
            {
                to_visit_users.push(item_neighbors[visit_item][j]);
            }
            visited_items.emplace(visit_item);
        }
    }

    std::vector<uint> user_map_;
    user_map_.assign(visited_users.begin(), visited_users.end());
    std::vector<uint> item_map_;
    item_map_.assign(visited_items.begin(), visited_items.end());

    return {user_map_, item_map_};
}

std::tuple<std::vector<uint>, std::vector<uint>>  Graph::Get2rHopOfUserByBV(uint center_user_id, uint r, std::bitset<MAX_LABEL> bv)
{
    std::queue<uint> to_visit_users;
    std::set<uint> visited_users;
    std::set<uint> qualified_user_set;
    std::queue<uint> to_visit_items;
    std::set<uint> visited_items;
    std::set<uint> qualified_item_set;

    to_visit_users.emplace(center_user_id);
    for (uint i=0;i < r; i++)
    {
        while (!to_visit_users.empty())
        {
            uint visit_user = to_visit_users.front();
            to_visit_users.pop();
            if (visited_users.find(visit_user) != visited_users.end()) continue;
            for (size_t j = 0; j < user_neighbors[visit_user].size(); j++)
            {
                to_visit_items.push(user_neighbors[visit_user][j]);
            }
            visited_users.emplace(visit_user);
            if ((bv & this->GetUserBv(visit_user)).any())
                qualified_user_set.emplace(visit_user);
        }

        while (!to_visit_items.empty())
        {
            uint visit_item = to_visit_items.front();
            to_visit_items.pop();
            if (visited_items.find(visit_item) != visited_items.end()) continue;
            
            for (size_t j = 0; j < item_neighbors[visit_item].size(); j++)
            {
                to_visit_users.push(item_neighbors[visit_item][j]);
            }
            visited_items.emplace(visit_item);
            if ((bv & this->GetItemBv(visit_item)).any())
                qualified_item_set.emplace(visit_item);
        }
    }

    std::vector<uint> user_map_;
    user_map_.assign(qualified_user_set.begin(), qualified_user_set.end());
    std::vector<uint> item_map_;
    item_map_.assign(qualified_item_set.begin(), qualified_item_set.end());

    return {user_map_, item_map_};
}


std::vector<InsertUnit> Graph::GetUpdateStream() const { return this->updates_; }

void Graph::LoadInitialGraph(const std::string &path)
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
    uint from_id=0, to_id=0, initial_timestamp=0;
    while (!ifs.eof())
    {
        {
            ifs >> from_id >> to_id >> initial_timestamp;
            uint addition_flag = InsertEdge(from_id, to_id);
            // if (addition_flag == 1)
            // {
            //     std::cout << "Added new edge: (" << from_id << "," << to_id << ")" << "\n";
            // }
            // else
            // {
            //     std::cout << "Inserted new edge: (" << from_id << "," << to_id << ")" << "\n";
            // }
            MaintainAfterInsertion(from_id, to_id, addition_flag);
        }
    }
    ifs.close();
    this->graph_timestamp = initial_timestamp;
}

void Graph::LoadItemLabel(const std::string &path)
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
    ifs >> this->label_size;
    // int counter = 0;
    while (!ifs.eof())
    {
        uint item_id;
        std::string label_str;
        ifs >> item_id >> label_str;
        // std::cout << "Item<" << item_id << "> has keywords (" << label_str << ")" << "\n";
        SetItemLabels(item_id, label_str);
        // counter++;
    }
    ifs.close();
}

void Graph::LoadUpdateStream(const std::string &path)
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
    std::string line_str;
    while (std::getline(ifs, line_str))
    {
        if (line_str.find('%') != line_str.npos) continue;
        std::stringstream ss(line_str);
        uint from_id, to_id, timestamp;
        ss >> from_id >> to_id >> timestamp;
        updates_.emplace_back(from_id, to_id, timestamp);
    }
    ifs.close();

    updates_.shrink_to_fit();
}

void Graph::PrintMetaData() const
{
    std::cout << "# user vertices = " << UserVerticesNum() << "\n" <<
        "# item vertices = " << ItemVerticesNum() << "\n" <<
        "# edges = " << NumEdges() << std::endl;
}