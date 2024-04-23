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

Graph::Graph()
: edge_count_(0)
, user_neighbors{}
, item_neighbors{}
, edge_weights{}
, updates_{}
, user_bvs{}
, item_bvs{}
, user_ub_sups{}
, user_neighbor_datas{}
{}

/// @brief add a new user vertex into graph
/// @param user_id 
void Graph::AddUserVertex(uint user_id)
{
    if (user_id >= user_neighbors.size())
    {
        user_neighbors.resize(user_id + 1);
        edge_weights.resize(user_id + 1);
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
        edge_weights[user_id][dis] ++;
        return 0;
    }
    // 3.2 else add edge into the user neighbors and item neighbors
    user_neighbors[user_id].insert(lower, item_id);
    edge_weights[user_id].insert(edge_weights[user_id].begin() + dis, 1);

    lower = std::lower_bound(item_neighbors[item_id].begin(), item_neighbors[item_id].end(), user_id);
    item_neighbors[item_id].insert(lower, user_id);
    edge_count_++;
    return 1;
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
    if (lower == user_neighbors[user_id].end() || *lower != item_id)
    {
        std::cout << "Edge Deletion Error: The edge to be deleted does not exist!" << std::endl;
        exit(-1);
    }
    // 1.2. weight-- if weight > 1
    size_t dis = std::distance(edge_weights[user_id].begin(), lower);
    edge_weights[user_id][dis] = edge_weights[user_id][dis] - 1;
    // edge_weights[user_id].erase(edge_weights[user_id].begin() + );
    if (edge_weights[user_id][dis] > 0)
    {
        return 0;
    }
    else if (edge_weights[user_id][dis] < 0)
    {
        std::cout << "Edge Deletion Error: The edge to be deleted with weight 0!" << std::endl;
        exit(-1);
    }
    // 2. remove the edge if weight == 1
    // 2.1. delete the item from user neighbor list
    user_neighbors[user_id].erase(lower);
    
    // 2.2 delete the user from item neighbor list
    lower = std::lower_bound(item_neighbors[item_id].begin(), item_neighbors[item_id].end(), user_id);
    if (lower == item_neighbors[item_id].end() || *lower != user_id)
    {
        std::cout << "deletion error" << std::endl;
        exit(-1);
    }
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

uint Graph::GetEdgeWeight(uint user_id, uint item_id) const
{
    const std::vector<uint> *nbrs = &GetUserNeighbors(user_id);
    const std::vector<uint> *user_edge_weights = &edge_weights[user_id];
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
    return -1;
}

void Graph::SetItemLabels(uint item_id, std::string label_str)
{
    uint bitvector = 0;
    std::stringstream ss(label_str); // 将input_str转换为stringstream格式
    std::stringstream trans;
    std::string label;
    while (std::getline(ss, label, ',')) // 以逗号为分隔符提取子字符串
    {
        std::stringstream trans(label);
        uint label_num;
        trans >> label_num;
        bitvector = bitvector | (1 << label_num);
    }
    AddItemVertex(item_id);
    item_bvs[item_id] = bitvector;
}


void Graph::LoadInitialGraph(const std::string &path)
{
    if (!io::file_exists(path.c_str()))
    {
        std::cout << "Unknown File Error: The input <" << path  << "> file does not exists"<< std::endl;
        exit(-1);
    }
    std::ifstream ifs(path);
    if (!ifs)
    {
        std::cout << "File Stream Error: The input file stream open failed" << std::endl;
        exit(-1);
    }
    while (!ifs.eof())
    {
        {
            uint from_id, to_id;
            ifs >> from_id >> to_id;
            uint result = InsertEdge(from_id, to_id);
        }
    }
    ifs.close();
}

void Graph::LoadItemLabel(const std::string &path)
{
    if (!io::file_exists(path.c_str()))
    {
        std::cout << "Unknown File Error: The input <" << path  << "> file does not exists"<< std::endl;
        exit(-1);
    }
    std::ifstream ifs(path);
    if (!ifs)
    {
        std::cout << "File Stream Error: The input file stream open failed" << std::endl;
        exit(-1);
    }
    while (!ifs.eof())
    {
        {
            uint item_id;
            std::string label_str;
            ifs >> item_id >> label_str;
            SetItemLabels(item_id, label_str);
        }
    }
    ifs.close();
}

void Graph::LoadUpdateStream(const std::string &path)
{
    if (!io::file_exists(path.c_str()))
    {
        std::cout << "Unknown File Error: The input <" << path  << "> file does not exists"<< std::endl;
        exit(-1);
    }
    std::ifstream ifs(path);
    if (!ifs)
    {
        std::cout << "File Stream Error: The input file stream open failed" << std::endl;
        exit(-1);
    }
    while (!ifs.eof())
    {
        uint from_id, to_id, timestamp;
        ifs >> from_id >> to_id >> timestamp;
        updates_.emplace_front(from_id, to_id, timestamp);
    }
    ifs.close();
}

void Graph::PrintMetaData() const
{
    std::cout << "# vertices = " << UserVerticesNum() <<
        "\n# edges = " << NumEdges() << std::endl;
}