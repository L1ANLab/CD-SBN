#include <algorithm>
#include <cmath>
#include <vector>
#include <set>

#include "utils/types.h"
#include "graph/graph.h"
#include "graph/induced_graph.h"

InducedGraph::InducedGraph()
: graph{}
, user_map{}
, item_map{}
, e_lists{}
{}

InducedGraph::InducedGraph(const Graph& g, uint user1, uint user2, uint item1, uint item2)
: graph(g)
, user_map{std::min(user1, user2), std::max(user1, user2)}
, item_map{std::min(item1, item2), std::max(item1, item2)}
, e_lists{{user1, item1}, {user1, item2}, {user2, item1}, {user2, item2}}
{}

InducedGraph::InducedGraph(InducedGraph& g1, InducedGraph& g2, bool is_union)
: graph(g1.graph)
, user_map(g1.user_map.size() + g2.user_map.size())
, item_map(g1.item_map.size() + g2.item_map.size())
, e_lists(g1.e_lists.size() + g2.e_lists.size())
{
    if (is_union)
    {
        std::vector<uint>::iterator it1 = std::set_union(
            g1.user_map.begin(), g1.user_map.end(),
            g2.user_map.begin(), g2.user_map.end(),
            user_map.begin()
        );
        user_map.resize(it1 - user_map.begin());

        std::vector<uint>::iterator it2 = std::set_union(
            g1.item_map.begin(), g1.item_map.end(),
            g2.item_map.begin(), g2.item_map.end(),
            item_map.begin()
        );
        item_map.resize(it2 - item_map.begin());

        std::vector<std::pair<uint, uint>>::iterator it3 = std::set_union(
            g1.e_lists.begin(), g1.e_lists.end(),
            g2.e_lists.begin(), g2.e_lists.end(),
            e_lists.begin()
        );
        e_lists.resize(it3 - e_lists.begin());
    }
    else
    {
        std::vector<uint>::iterator it1 = std::set_intersection(
            g1.user_map.begin(), g1.user_map.end(),
            g2.user_map.begin(), g2.user_map.end(),
            user_map.begin()
        );
        user_map.resize(it1 - user_map.begin());

        std::vector<uint>::iterator it2 = std::set_intersection(
            g1.item_map.begin(), g1.item_map.end(),
            g2.item_map.begin(), g2.item_map.end(),
            item_map.begin()
        );
        item_map.resize(it2 - item_map.begin());

        std::vector<std::pair<uint, uint>>::iterator it3 = std::set_intersection(
            g1.e_lists.begin(), g1.e_lists.end(),
            g2.e_lists.begin(), g2.e_lists.end(),
            e_lists.begin()
        );
        e_lists.resize(it3 - e_lists.begin());
    }
}

InducedGraph::InducedGraph(
    const Graph& g,
    std::vector<uint> user_map_,
    std::vector<uint> item_map_
): graph(g)
, user_map(user_map_)
, item_map(item_map_)
, e_lists{}
{
    for(uint user: user_map_)
    {
        std::vector<uint> user_neighbors = g.GetUserNeighbors(user);
        for (uint neighbor_item: user_neighbors)
        {
            if (std::binary_search(item_map_.begin(), item_map_.end(), neighbor_item))
            {
                e_lists.push_back(std::pair(user, neighbor_item));
            }
        }
        e_lists.resize(e_lists.size());
    }
}

InducedGraph::InducedGraph(
    const Graph& g,
    std::vector<uint> user_map_,
    std::vector<uint> item_map_,
    std::vector<std::pair<uint, uint>> e_lists_
): graph(g)
, user_map(user_map_)
, item_map(item_map_)
, e_lists(e_lists_)
{ }

uint InducedGraph::GetUserDegree(uint user_id)
{
    uint degree = 0u;

    for (auto& e: e_lists)
        if (e.first == user_id)
            degree++;

    return degree;
}


InducedGraph* InducedGraph::ComputeBitruss(uint k)
{
    // std::vector<uint> user_list;
    // for (uint user : this->user_map)
    // {
    //     if ((bv & this->graph.GetUserBv(user)).none())
    //         continue;
    //     user_list.emplace_back(user);
    // }
    // user_list.shrink_to_fit();

    // std::vector<uint> item_list;
    // for (uint item : this->item_map)
    // {
    //     if ((bv & this->graph.GetItemBv(item)).none())
    //         continue;
    //     item_list.emplace_back(item);
    // }
    // item_list.shrink_to_fit();

    // std::vector<uint> new_edge_list;
    // for (std::pair<uint, uint> edge : this->e_lists)
    // {
    //     if (!std::binary_search(user_list.begin(), user_list.end(), edge.first))
    //         continue;
    //     if (!std::binary_search(item_list.begin(), item_list.end(), edge.second))
    //         continue;
    //     new_edge_list.emplace_back(edge);
    // }
    // item_list.shrink_to_fit();

    std::vector<uint> user_list(this->user_map);
    // 0.use adjacency list to save the edges
    std::vector<std::vector<std::pair<uint, uint>>> new_edge_list;
    new_edge_list.resize(*std::max_element(user_list.begin(), user_list.end()));
    
    for (auto edge : e_lists)
    {
        new_edge_list[edge.first].emplace_back(std::pair{edge.second, 0});
    }

    // 1. compute the support for each edge in the subgraph
    for (uint user : user_list)
    {
        // std::vector<uint> item_neighbors;
        // for ()
        // std::copy_if(
        //     this->graph.GetUserNeighbors(user).begin(),
        //     this->graph.GetUserNeighbors(user).end(),
        //     item_neighbors.begin(),
        //     [item_list] (const uint user_neighbor){
        //         return (user_neighbor > user &&
        //         std::binary_search(item_list.begin(), user_list.end(), user_neighbor));
        //     }
        // );
        for (size_t i = 0; i< new_edge_list[user].size(); i++)
        {
            uint item_neighbor1 = new_edge_list[user][i].first;
            for (size_t j = i+1;j<new_edge_list[user].size();j++)
            {
                // 1.1, for the choosed two item, find common neighor user.
                uint item_neighbor2 = new_edge_list[user][j].first;
                std::vector<uint> raw_common_neighbors;
                std::set_intersection(
                    this->graph.GetItemNeighbors(item_neighbor1).begin(),
                    this->graph.GetItemNeighbors(item_neighbor1).end(),
                    this->graph.GetItemNeighbors(item_neighbor2).begin(),
                    this->graph.GetItemNeighbors(item_neighbor2).end(),
                    raw_common_neighbors.begin()
                );
                raw_common_neighbors.shrink_to_fit();
                std::vector<uint> common_neighbors;
                std::copy_if(
                    raw_common_neighbors.begin(),
                    raw_common_neighbors.end(),
                    common_neighbors.begin(),
                    [user, user_list] (uint common_neighbor) {
                        return (common_neighbor > user &&
                        std::binary_search(user_list.begin(), user_list.end(), common_neighbor));
                    }
                );
                // 1.2. increase the support of butterfly
                for (uint another_user :common_neighbors)
                {
                    new_edge_list[user][i].second ++;
                    new_edge_list[user][j].second ++;
                    int count = 0;
                    for (auto edge : new_edge_list[another_user])
                    {
                        if (edge.first == item_neighbor1 || edge.first == item_neighbor2)
                        {
                            edge.second ++;
                            count ++;
                        }
                        if (count == 2) break;
                    }
                }
            }
        }
    }

    // 2. compute the bitruss
    // 2.1. remove and update edges until qualified
    uint drop_edge_num = 1;
    while(drop_edge_num > 0)
    {
        drop_edge_num = 0;
        std::vector<std::pair<uint, uint>> drop_edge_list;
        std::set<uint> visited_user;
        // (1) detect all unqualified edge
        for (size_t user=0;user< user_list.size(); user++)
        {
            for (size_t idx=0; idx<new_edge_list[user].size();idx++)
            {
                if (new_edge_list[user][idx].second < k)
                {
                    drop_edge_list.emplace_back(std::pair(user, idx));
                }
            }
        }
        // (2) remove the detection edges and update the related edges
        for (auto drop_edge_info : drop_edge_list)
        {
            uint user = drop_edge_info.first;
            uint item_idx = drop_edge_info.second;
            uint item = new_edge_list[user][item_idx].second;
            for (size_t i = 0; i< new_edge_list[user].size(); i++)
            {
                uint item_neighbor = new_edge_list[user][i].first;
                if (item_neighbor == item) continue;

                std::vector<uint> raw_common_neighbors;
                std::set_intersection(
                    this->graph.GetItemNeighbors(item).begin(),
                    this->graph.GetItemNeighbors(item).end(),
                    this->graph.GetItemNeighbors(item_neighbor).begin(),
                    this->graph.GetItemNeighbors(item_neighbor).end(),
                    raw_common_neighbors.begin()
                );
                raw_common_neighbors.shrink_to_fit();
                std::vector<uint> common_neighbors;
                std::copy_if(
                    raw_common_neighbors.begin(),
                    raw_common_neighbors.end(),
                    common_neighbors.begin(),
                    [user_list] (uint common_neighbor) {
                        return std::binary_search(user_list.begin(), user_list.end(), common_neighbor);
                    }
                );
                // 1.2. increase the support of butterfly
                for (uint another_user :common_neighbors)
                {
                    new_edge_list[user][i].second --;
                    int count = 0;
                    for (auto edge : new_edge_list[another_user])
                    {
                        if (edge.first == item_neighbor || edge.first == item)
                        {
                            edge.second --;
                            count --;
                        }
                        if (count == 2) break;
                    }
                }
            }
            new_edge_list[user].erase(new_edge_list[user].begin()+item_idx);
            if (new_edge_list[user].empty())
                user_list.erase(std::lower_bound(user_list.begin(), user_list.end(), user));
        }
        // (3) update drop_num
        drop_edge_num = drop_edge_list.size();
    }

    std::set<uint> item_set;
    std::vector<std::pair<uint, uint>> new_e_list;
    for (size_t i=0; i<new_edge_list.size();i++)
    {
        for (auto edge : new_edge_list[i])
        {
            item_set.emplace(edge.first);
            new_e_list.emplace_back(std::pair(i, edge.first));
        }
    }
    new_e_list.shrink_to_fit();
    std::vector<uint> item_list;
    item_list.assign(item_set.begin(), item_set.end());

    return new InducedGraph(this->graph, user_list, item_list, new_e_list);
}
