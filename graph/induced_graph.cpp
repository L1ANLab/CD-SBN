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


bool InducedGraph::operator < (const InducedGraph& subgraph) const
{
    if (this->NumUsers() == subgraph.NumUsers())
    {
        if (this->NumItems() == subgraph.NumItems())
        {
            return this->NumEdges() < subgraph.NumEdges();
        }
        return this->NumItems() < subgraph.NumItems();
    }
    return this->NumUsers() < subgraph.NumUsers();
}

bool InducedGraph::operator == (const InducedGraph& subgraph) const
{
    return (
        this->user_map == subgraph.user_map &&
        this->item_map == subgraph.item_map &&
        this->e_lists == subgraph.e_lists
    );
}



uint InducedGraph::GetUserDegree(uint user_id)
{
    uint degree = 0u;

    for (auto& e: e_lists)
        if (e.first == user_id)
            degree++;

    return degree;
}


/// @brief obtain a K-bitruss from current r-hop subgraph
/// @param k uint type: the order of the bitruss
/// @return InducedGraph type: a maximal k-bitruss of current subgraph
InducedGraph* InducedGraph::ComputeKBitruss(uint k)
{
    std::vector<uint> user_list(this->user_map);
    // 0.use adjacency list to save the edges
    std::vector<std::vector<std::pair<uint, uint>>> new_edge_list(user_list.back()+1);
    
    for (auto edge : this->e_lists)
    {
        new_edge_list[edge.first].emplace_back(std::pair{edge.second, 0});
    }

    // 1. compute the support for each edge in the subgraph
    for (uint user : user_list)
    {
        for (size_t i = 0; i< new_edge_list[user].size(); i++)
        {
            uint item_neighbor1 = new_edge_list[user][i].first;
            for (size_t j = i+1;j<new_edge_list[user].size();j++)
            {
                // 1.1, for the choosed two item, find common neighor user.
                uint item_neighbor2 = new_edge_list[user][j].first;
                std::vector<uint> raw_common_neighbors(
                    this->graph.GetItemNeighbors(item_neighbor1).size()+
                    this->graph.GetItemNeighbors(item_neighbor2).size()
                );
                std::vector<uint>::iterator it = std::set_intersection(
                    this->graph.GetItemNeighbors(item_neighbor1).begin(),
                    this->graph.GetItemNeighbors(item_neighbor1).end(),
                    this->graph.GetItemNeighbors(item_neighbor2).begin(),
                    this->graph.GetItemNeighbors(item_neighbor2).end(),
                    raw_common_neighbors.begin()
                );
                raw_common_neighbors.resize(it - raw_common_neighbors.begin());
                std::vector<uint> common_neighbors(raw_common_neighbors.size());
                std::vector<uint>::iterator copy_iter = std::copy_if(
                    raw_common_neighbors.begin(),
                    raw_common_neighbors.end(),
                    common_neighbors.begin(),
                    [user, user_list] (uint common_neighbor) {
                        return (common_neighbor > user &&
                        std::binary_search(user_list.begin(), user_list.end(), common_neighbor));
                    }
                );
                common_neighbors.resize(copy_iter - common_neighbors.begin());
                // 1.2. increase the support of butterfly
                for (uint another_user :common_neighbors)
                {
                    new_edge_list[user][i].second ++;
                    new_edge_list[user][j].second ++;
                    int count = 0;
                    for (uint idx = 0; idx < new_edge_list[another_user].size(); idx++)
                    {
                        if (new_edge_list[another_user][idx].first == item_neighbor1 ||
                            new_edge_list[another_user][idx].first == item_neighbor2)
                        {
                            new_edge_list[another_user][idx].second ++;
                            count ++;
                        }
                        if (count == 2) break;
                    }
                }
            }
        }
    }
    // Print_Time_Now("Compute Support: ", start);

    // 2. compute the bitruss
    // 2.1. remove and update edges until qualified
    uint drop_edge_num = 1;
    // start = Get_Time();
    while(drop_edge_num > 0)
    {
        drop_edge_num = 0;
        bool hasEdge = false;
        std::vector<std::pair<uint, uint>> drop_edge_list;
        // (1) detect all unqualified edge
        for (size_t user=0;user< user_list.size(); user++)
        {
            for (size_t idx=0; idx<new_edge_list[user].size();idx++)
            {
                // pass the qualified edge and 0-sup edge (UINT_MAX)
                if (new_edge_list[user][idx].second >= k)
                {
                    hasEdge = true;
                    continue;
                }
                drop_edge_list.emplace_back(std::pair(user, idx));
            }
        }
        if (!hasEdge) break; // if no edge
        // (2) remove the detection edges and update the related edges
        for (auto drop_edge_info : drop_edge_list)
        {
            uint user = drop_edge_info.first;
            uint item_idx = drop_edge_info.second;
            uint item = new_edge_list[user][item_idx].first;
            for (size_t i = 0; i< new_edge_list[user].size(); i++)
            {
                uint item_neighbor = new_edge_list[user][i].first;
                if (item_neighbor == item) continue;

                std::vector<uint> raw_common_neighbors(
                    this->graph.GetItemNeighbors(item).size() +
                    this->graph.GetItemNeighbors(item_neighbor).size()
                );
                std::vector<uint>::iterator it = std::set_intersection(
                    this->graph.GetItemNeighbors(item).begin(),
                    this->graph.GetItemNeighbors(item).end(),
                    this->graph.GetItemNeighbors(item_neighbor).begin(),
                    this->graph.GetItemNeighbors(item_neighbor).end(),
                    raw_common_neighbors.begin()
                );
                raw_common_neighbors.resize(it - raw_common_neighbors.begin());
                std::vector<uint> common_neighbors(raw_common_neighbors.size());
                std::vector<uint>::iterator copy_iter = std::copy_if(
                    raw_common_neighbors.begin(),
                    raw_common_neighbors.end(),
                    common_neighbors.begin(),
                    [user_list] (uint common_neighbor) {
                        return std::binary_search(user_list.begin(), user_list.end(), common_neighbor);
                    }
                );
                common_neighbors.resize(copy_iter - common_neighbors.begin());
                // 1.2. increase the support of butterfly
                for (uint another_user :common_neighbors)
                {
                    new_edge_list[user][i].second --;
                    int count = 0;
                    for (uint idx = 0; idx < new_edge_list[another_user].size(); idx++)
                    {
                        if (new_edge_list[another_user][idx].first == item_neighbor ||
                            new_edge_list[another_user][idx].first == item)
                        {
                            new_edge_list[another_user][idx].second --;
                            count ++;
                        }
                        if (count == 2) break;
                    }
                }
            }
            // new_edge_list[user].erase(new_edge_list[user].begin()+item_idx);
            new_edge_list[user][item_idx].second = UINT_MAX;
            if (new_edge_list[user].empty())
                user_list.erase(std::lower_bound(user_list.begin(), user_list.end(), user));
        }
        // (3) update drop_num
        drop_edge_num = drop_edge_list.size();
    }
    // Print_Time_Now("Filter Support: ", start);

    std::set<uint> item_set;
    std::vector<std::pair<uint, uint>> new_e_list;
    for (size_t i=0; i<new_edge_list.size();i++)
    {
        for (auto edge : new_edge_list[i])
        {
            if (edge.second != UINT_MAX)
            {
                item_set.emplace(edge.first);
                new_e_list.emplace_back(std::pair(i, edge.first));
            }
        }
    }
    new_e_list.shrink_to_fit();
    std::vector<uint> item_list;
    item_list.assign(item_set.begin(), item_set.end());

    return new InducedGraph(this->graph, user_list, item_list, new_e_list);
}


/// @brief obtain a (K, r, \sigma)-bitruss from current (k,r)-bitruss subgraph
/// @param sigma 
/// @return InducedGraph type: a maximal (K, r, \sigma)-bitruss of current subgraph
InducedGraph* InducedGraph::ComputeSigmaBitruss(uint sigma)
{
    std::vector<uint> user_list(this->user_map);
    // 0.use adjacency list to save the edges
    std::vector<std::vector<std::pair<uint, uint>>> user_relationship_score_list(user_list.back()+1);

    std::vector<std::vector<uint>> user_neighbor_list(user_list.back()+1);
    
    for (auto edge : e_lists)
    {
        auto insert_pos = std::lower_bound(
            user_neighbor_list[edge.first].begin(),
            user_neighbor_list[edge.first].end(),
            edge.second
        );
        user_neighbor_list[edge.first].insert(insert_pos, edge.second);
    }

    // 1. compute the user relationship score for each edge in the subgraph
    for (size_t i = 0; i< user_list.size(); i++)
    {
        uint user1 = user_list[i];
        for (size_t j = i+1;j<user_list.size();j++)
        {
            // 1.1. for the choosed two item, find common neighbor users
            uint user2 = user_list[j];
            std::vector<uint> raw_common_neighbors(
                user_neighbor_list[user1].size() +
                user_neighbor_list[user2].size()
            );
            std::vector<uint>::iterator it = std::set_intersection(
                user_neighbor_list[user1].begin(),
                user_neighbor_list[user1].end(),
                user_neighbor_list[user2].begin(),
                user_neighbor_list[user2].end(),
                raw_common_neighbors.begin()
            );
            raw_common_neighbors.resize(it - raw_common_neighbors.begin());
            // skip to next user if not common neighbor
            if (raw_common_neighbors.empty()) continue;
            // 1.2. compute the user relationship score
            uint sum = 0, square_sum = 0;
            for (uint common_item : raw_common_neighbors)
            {
                uint wedge_score = std::min(
                    this->graph.GetEdgeData(user1, common_item)->weight,
                    this->graph.GetEdgeData(user2, common_item)->weight
                );
                sum += wedge_score;
                square_sum += wedge_score*wedge_score;
            }
            // user_relationship_score_list[user1].emplace_back(
            //     std::pair(user2, (sum*sum-square_sum)/2)
            // );
            // user_relationship_score_list[user2].emplace_back(
            //     std::pair(user1, (sum*sum-square_sum)/2)
            // );
            if ((sum*sum-square_sum)/2 < sigma)
            {
                return new InducedGraph(this->graph, {}, {}, {});
            }
        }
    }

    /*
    // 2. compute the bitruss
    // 2.1. remove and update edges until qualified
    uint drop_user_num = 1;
    while(drop_user_num > 0)
    {
        drop_user_num = 0;
        std::vector<uint> drop_user_list;
        // (1) detect all unqualified edge
        for (size_t user=0;user<user_relationship_score_list.size(); user++)
        {
            for (size_t idx=0; idx<user_relationship_score_list[user].size();idx++)
            {
                if (user_relationship_score_list[user][idx].second < sigma)
                {
                    drop_user_list.emplace_back(std::pair(user, idx));
                }
            }
        }
        // (2) remove the detection edges and update the related edges
        // (3) update drop_num
        drop_user_num = drop_user_list.size();
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
    */

    return new InducedGraph(this->graph, this->user_map, this->item_map, this->e_lists);
}




InducedGraph* InducedGraph::ComputeKRSigmaBitruss(uint k, uint sigma)
{
    std::vector<uint> user_list(this->user_map);
    // 0.use adjacency list to save the edges
    std::vector<std::vector<std::pair<uint, uint>>> edge_support_list(user_list.back()+1);
    for (auto edge : this->e_lists)
    {
        edge_support_list[edge.first].emplace_back(std::pair{edge.second, 0});
    }

    // 1. compute the support for each edge in the subgraph
    for (uint user : user_list)
    {
        for (size_t i = 0; i< edge_support_list[user].size(); i++)
        {
            uint item_neighbor1 = edge_support_list[user][i].first;
            for (size_t j = i+1;j<edge_support_list[user].size();j++)
            {
                // 1.1, for the choosed two item, find common neighor user.
                uint item_neighbor2 = edge_support_list[user][j].first;
                std::vector<uint> raw_common_neighbors(
                    this->graph.GetItemNeighbors(item_neighbor1).size()+
                    this->graph.GetItemNeighbors(item_neighbor2).size()
                );
                std::vector<uint>::iterator it = std::set_intersection(
                    this->graph.GetItemNeighbors(item_neighbor1).begin(),
                    this->graph.GetItemNeighbors(item_neighbor1).end(),
                    this->graph.GetItemNeighbors(item_neighbor2).begin(),
                    this->graph.GetItemNeighbors(item_neighbor2).end(),
                    raw_common_neighbors.begin()
                );
                raw_common_neighbors.resize(it - raw_common_neighbors.begin());
                std::vector<uint> common_neighbors(raw_common_neighbors.size());
                std::vector<uint>::iterator copy_iter = std::copy_if(
                    raw_common_neighbors.begin(),
                    raw_common_neighbors.end(),
                    common_neighbors.begin(),
                    [user, user_list] (uint common_neighbor) {
                        return (common_neighbor > user &&
                        std::binary_search(user_list.begin(), user_list.end(), common_neighbor));
                    }
                );
                common_neighbors.resize(copy_iter - common_neighbors.begin());
                // 1.2. increase the support of butterfly
                for (uint another_user :common_neighbors)
                {
                    edge_support_list[user][i].second ++;
                    edge_support_list[user][j].second ++;
                    int count = 0;
                    for (uint idx = 0; idx < edge_support_list[another_user].size(); idx++)
                    {
                        if (edge_support_list[another_user][idx].first == item_neighbor1 ||
                            edge_support_list[another_user][idx].first == item_neighbor2)
                        {
                            edge_support_list[another_user][idx].second ++;
                            count ++;
                        }
                        if (count == 2) break;
                    }
                }
            }
        }
    }


    // 2. compute the user relationship score for each pair of user in the subgraph
    std::vector<std::vector<std::pair<uint, uint>>> user_score_list(user_list.back()+1);

    std::vector<std::vector<uint>> user_neighbor_list(user_list.back()+1);
    for (auto edge : e_lists)
    {
        auto insert_pos = std::lower_bound(
            user_neighbor_list[edge.first].begin(),
            user_neighbor_list[edge.first].end(),
            edge.second
        );
        user_neighbor_list[edge.first].insert(insert_pos, edge.second);
    }

    for (size_t i = 0; i< user_list.size(); i++)
    {
        uint user1 = user_list[i];
        for (size_t j = i+1;j<user_list.size();j++)
        {
            // 1.1. for the choosed two item, find common neighbor users
            uint user2 = user_list[j];
            std::vector<uint> raw_common_neighbors(
                user_neighbor_list[user1].size() +
                user_neighbor_list[user2].size()
            );
            std::vector<uint>::iterator it = std::set_intersection(
                user_neighbor_list[user1].begin(),
                user_neighbor_list[user1].end(),
                user_neighbor_list[user2].begin(),
                user_neighbor_list[user2].end(),
                raw_common_neighbors.begin()
            );
            raw_common_neighbors.resize(it - raw_common_neighbors.begin());
            // skip to next user if not common neighbor
            if (raw_common_neighbors.empty()) continue;
            // 1.2. compute the user relationship score
            uint sum = 0, square_sum = 0;
            for (uint common_item : raw_common_neighbors)
            {
                uint wedge_score = std::min(
                    this->graph.GetEdgeData(user1, common_item)->weight,
                    this->graph.GetEdgeData(user2, common_item)->weight
                );
                sum += wedge_score;
                square_sum += wedge_score*wedge_score;
            }
            user_score_list[user1].emplace_back(
                std::pair(user2, (sum*sum-square_sum)/2)
            );
            user_score_list[user2].emplace_back(
                std::pair(user1, (sum*sum-square_sum)/2)
            );
        }
    }


    // user_neighbor_list[user_id] = [neighbor_item_id]
    // edge_support_list[user_id] = [<neighbor_item_id, sup>]
    // user_score_list[user_id] = [<2-neighbor_user_id, score>]
    // 3. compute the bitruss: remove and update edges until qualified
    uint drop_edge_num = 1;
    while(drop_edge_num > 0)
    {
        drop_edge_num = 0;
        std::vector<std::pair<uint, uint>> drop_edge_list;
        // (1) detect all unqualified edge by support
        for (size_t user=0;user< user_list.size(); user++)
        {
            for (size_t idx=0; idx<edge_support_list[user].size();idx++)
            {
                // pass the qualified edge and 0-sup edge (UINT_MAX)
                if (edge_support_list[user][idx].second >= k)
                    continue;
                drop_edge_list.emplace_back(std::pair(user, idx));
            }
        }
        // (2) detect all unqualified edge by support
        // (3) remove the detection edges and update the related edges
        for (auto drop_edge_info : drop_edge_list)
        {
            uint user = drop_edge_info.first;
            uint item_idx = drop_edge_info.second;
            uint item = edge_support_list[user][item_idx].first;
            for (size_t i = 0; i< edge_support_list[user].size(); i++)
            {
                uint item_neighbor = edge_support_list[user][i].first;
                if (item_neighbor == item) continue;

                std::vector<uint> raw_common_neighbors(
                    this->graph.GetItemNeighbors(item).size() +
                    this->graph.GetItemNeighbors(item_neighbor).size()
                );
                std::vector<uint>::iterator it = std::set_intersection(
                    this->graph.GetItemNeighbors(item).begin(),
                    this->graph.GetItemNeighbors(item).end(),
                    this->graph.GetItemNeighbors(item_neighbor).begin(),
                    this->graph.GetItemNeighbors(item_neighbor).end(),
                    raw_common_neighbors.begin()
                );
                raw_common_neighbors.resize(it - raw_common_neighbors.begin());
                std::vector<uint> common_neighbors(raw_common_neighbors.size());
                std::vector<uint>::iterator copy_iter = std::copy_if(
                    raw_common_neighbors.begin(),
                    raw_common_neighbors.end(),
                    common_neighbors.begin(),
                    [user_list] (uint common_neighbor) {
                        return std::binary_search(user_list.begin(), user_list.end(), common_neighbor);
                    }
                );
                common_neighbors.resize(copy_iter - common_neighbors.begin());
                // recompute the support of edges in butterflies
                for (uint another_user :common_neighbors)
                {
                    edge_support_list[user][i].second --;
                    int count = 0;
                    for (uint idx = 0; idx < edge_support_list[another_user].size(); idx++)
                    {
                        if (edge_support_list[another_user][idx].first == item_neighbor ||
                            edge_support_list[another_user][idx].first == item)
                        {
                            edge_support_list[another_user][idx].second --;
                            count ++;
                        }
                        if (count == 2) break;
                    }
                }
            }
            edge_support_list[user].erase(edge_support_list[user].begin()+item_idx);
            if (edge_support_list[user].empty())
                user_list.erase(std::lower_bound(user_list.begin(), user_list.end(), user));
        }

        // (4) update drop_num
        drop_edge_num = drop_edge_list.size();
    }

    std::set<uint> item_set;
    std::vector<std::pair<uint, uint>> new_e_list;
    for (size_t i=0; i<edge_support_list.size();i++)
    {
        for (auto edge : edge_support_list[i])
        {
            if (edge.second != UINT_MAX)
            {
                item_set.emplace(edge.first);
                new_e_list.emplace_back(std::pair(i, edge.first));
            }
        }
    }
    new_e_list.shrink_to_fit();
    std::vector<uint> item_list;
    item_list.assign(item_set.begin(), item_set.end());

    return new InducedGraph(this->graph, user_list, item_list, new_e_list);
}

std::string InducedGraph::PrintMetaData()
{
    std::string result_str;
    result_str += "User List[" + std::to_string(user_map.size()) +  "]: ";
    for (uint user: user_map)
        result_str += std::to_string(user) + " ";
    result_str += "\n";
    result_str += "Item List[" + std::to_string(item_map.size()) +  "]: ";
    for (uint item: item_map)
        result_str += std::to_string(item) + " ";
    result_str += "\n";
    result_str += "edge List[" + std::to_string(e_lists.size()) +  "]";
    result_str += "\n";
    return result_str;
}
