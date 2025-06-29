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
, user_neighbor_datas{}
{}

InducedGraph::InducedGraph(const Graph& g, uint user1, uint user2, uint item1, uint item2)
: graph(g)
, user_map{std::min(user1, user2), std::max(user1, user2)}
, item_map{std::min(item1, item2), std::max(item1, item2)}
, e_lists{{user1, item1}, {user1, item2}, {user2, item1}, {user2, item2}}
, user_neighbor_datas{}
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

// InducedGraph::InducedGraph(
//     const Graph& g,
//     std::vector<uint> user_map_,
//     std::vector<uint> item_map_,
//     std::vector<std::pair<uint, uint>> e_lists_,
//     std::vector<std::vector<WedgeData *>> user_neighbor_datas_
// ): graph(g)
// , user_map(user_map_)
// , item_map(item_map_)
// , e_lists(e_lists_)
// , user_neighbor_datas{}
// {
//     // for(uint user: user_map_)
//     // {
//     //     std::vector<uint> user_neighbors = g.GetUserNeighbors(user);
//     //     for (uint neighbor_item: user_neighbors)
//     //     {
//     //         if (std::binary_search(item_map_.begin(), item_map_.end(), neighbor_item))
//     //         {
//     //             e_lists.push_back(std::pair(user, neighbor_item));
//     //         }
//     //     }
//     //     e_lists.resize(e_lists.size());
//     // }
// }



InducedGraph::InducedGraph(
    const Graph& g,
    std::vector<uint> user_map_,
    std::vector<uint> item_map_,
    std::vector<std::pair<uint, uint>> e_lists_,
    std::vector<std::vector<WedgeData *>> user_neighbor_datas_
): graph(g)
, user_map(user_map_)
, item_map(item_map_)
, e_lists(e_lists_)
, user_neighbor_datas{}
{
    // 0. initialize the edge list set and neighbor data
    user_neighbor_datas.resize(user_map_.back()+1);
    // 1. find the item neighbors (1-hop) and user neighbors (2-hop) for each subgraph user
    // uint count = 0;
    for(uint user: user_map_)
    {
        std::vector<UserData*> user_data = g.GetNeighborUserData(user);

        for (UserData* user_data_p : user_data)
        {
            if (user >= user_data_p->user_id) continue; // avoid double counting
            if (std::binary_search(user_map_.begin(), user_map_.end(), user_data_p->user_id))
            {
                // 3. if the 2-hop user neighbor in the whole graph is also in the subgraph
                std::vector<uint> common_wedge_item_list(0);

                // 3.1. find the common items between the user and the 2-hop user
                common_wedge_item_list.resize(
                    item_map_.size() +
                    user_data_p->wedge_score_list.size()
                );
                std::vector<uint>::iterator it = std::set_intersection(
                    item_map_.begin(),
                    item_map_.end(),
                    user_data_p->wedge_score_list.begin(),
                    user_data_p->wedge_score_list.end(),
                    common_wedge_item_list.begin()
                );
                common_wedge_item_list.resize(it - common_wedge_item_list.begin());

                if (common_wedge_item_list.size() < 1) continue;
                // if (common_wedge_item_list.size() >= 2) count += (common_wedge_item_list.size()-1);
                // 3.2. create a new UserData object and add it
                user_neighbor_datas[user].emplace_back(
                    new WedgeData(
                        user_data_p->user_id,
                        common_wedge_item_list
                    )
                );
            }
        }
    }
    // std::cout << "Support Count: " << count << std::endl;
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
{
    user_neighbor_datas.resize(0);
}


InducedGraph::~InducedGraph()
{
    
    std::vector<uint>().swap(user_map);
    std::vector<uint>().swap(item_map);
    std::vector<std::pair<uint, uint>>().swap(e_lists);
    for (uint i=0;i<user_neighbor_datas.size();i++)
    {
        for (uint j=0;j<user_neighbor_datas[i].size();j++)
        {
            delete user_neighbor_datas[i][j];
        }
        std::vector<WedgeData*>().swap(user_neighbor_datas[i]);
    }
    std::vector<std::vector<WedgeData*>>().swap(user_neighbor_datas);
}

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
    // 0.use adjacency list to save the edges
    std::vector<std::vector<std::pair<uint, uint>>> new_edge_list(this->user_map.back()+1);
    std::vector<std::vector<uint>> item_neighbor_list(item_map.back()+1);
    
    for (auto edge : this->e_lists)
    {
        new_edge_list[edge.first].emplace_back(std::pair{edge.second, 0});
        item_neighbor_list[edge.second].emplace_back(edge.first);
    }

    std::chrono::high_resolution_clock::time_point start = Get_Time();
    // 1. compute the support for each edge in the subgraph
    for (uint user : this->user_map)
    {
        for (size_t i = 0; i< new_edge_list[user].size(); i++)
        {
            uint item_neighbor1 = new_edge_list[user][i].first;
            for (size_t j = i+1;j<new_edge_list[user].size();j++)
            {
                // 1.1, for the choosed two item, find common neighor users.
                uint item_neighbor2 = new_edge_list[user][j].first;
                std::vector<uint> common_neighbors(
                    item_neighbor_list[item_neighbor1].size()+
                    item_neighbor_list[item_neighbor2].size()
                );
                std::vector<uint>::iterator it = std::set_intersection(
                    item_neighbor_list[item_neighbor1].begin(),
                    item_neighbor_list[item_neighbor1].end(),
                    item_neighbor_list[item_neighbor2].begin(),
                    item_neighbor_list[item_neighbor2].end(),
                    common_neighbors.begin()
                );
                common_neighbors.resize(it - common_neighbors.begin());

                // 1.2. for each common neighbor user, count the butterflies
                for (uint another_user :common_neighbors)
                {
                    if (another_user <= user) continue; // avoid double counting
                    new_edge_list[user][i].second ++;
                    new_edge_list[user][j].second ++;
                    int count = 0;
                    for (uint idx = 0; idx < new_edge_list[another_user].size(); idx++)
                    {
                        uint now_item = new_edge_list[another_user][idx].first;
                        if (now_item == item_neighbor1 || now_item == item_neighbor2)
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
    Print_Time_Now("Compute Support: ", start);
    // 2. compute the bitruss
    // detect all unqualified edge
    start = Get_Time();
    uint drop_edge_num = 1;

    while(drop_edge_num > 0)
    {
        drop_edge_num = 0;
        // detect all unqualified edge
        std::vector<std::pair<uint, uint>> drop_edge_list;
        for (uint user: this->user_map)
        {
            for (size_t idx=0; idx<new_edge_list[user].size();idx++)
            {
                if (new_edge_list[user][idx].second >0 && new_edge_list[user][idx].second < k)
                    drop_edge_list.emplace_back(std::pair(user, idx));
            }
        }
        drop_edge_list.shrink_to_fit();
        // 2.1. remove the detection edges and update the related edges
        for (auto drop_edge_info : drop_edge_list)
        {
            // (1) extract the drop edge info
            uint user = drop_edge_info.first;
            uint item_idx = drop_edge_info.second;
            if (new_edge_list[user][item_idx].second == 0)
                continue;
            uint item = new_edge_list[user][item_idx].first;
            for (size_t i = 0; i< new_edge_list[user].size(); i++)
            {
                if (new_edge_list[user][item_idx].second == 0) break;
                if (new_edge_list[user][i].second == 0) continue;
                uint item_neighbor = new_edge_list[user][i].first;
                if (item_neighbor == item) continue;

                std::vector<uint> common_neighbors(
                    item_neighbor_list[item].size() +
                    item_neighbor_list[item_neighbor].size()
                );
                std::vector<uint>::iterator it = std::set_intersection(
                    item_neighbor_list[item].begin(),
                    item_neighbor_list[item].end(),
                    item_neighbor_list[item_neighbor].begin(),
                    item_neighbor_list[item_neighbor].end(),
                    common_neighbors.begin()
                );
                common_neighbors.resize(it - common_neighbors.begin());

                if (common_neighbors.size() < 2) continue;
                // 1.2. increase the support of butterfly
                for (uint another_user : common_neighbors)
                {
                    if (another_user == user) continue;
                    // if (new_edge_list[user][item_idx].second == 0)
                    // {
                    //     std::cout << "Error Support Computation" << "\n"; 
                    // }
                    new_edge_list[user][item_idx].second --;
                    if (new_edge_list[user][item_idx].second == 0)
                    {
                        std::vector<uint>::iterator remove_it = std::remove(
                            item_neighbor_list[item].begin(),
                            item_neighbor_list[item].end(),
                            user
                        );
                        item_neighbor_list[item].resize(remove_it - item_neighbor_list[item].begin());
                    }
                    // if (new_edge_list[user][i].second == 0)
                    // {
                    //     std::cout << "Error Support Computation" << "\n"; 
                    // }
                    new_edge_list[user][i].second --;
                    if (new_edge_list[user][i].second == 0)
                    {
                        std::vector<uint>::iterator remove_it = std::remove(
                            item_neighbor_list[item_neighbor].begin(),
                            item_neighbor_list[item_neighbor].end(),
                            user
                        );
                        item_neighbor_list[item_neighbor].resize(remove_it - item_neighbor_list[item_neighbor].begin());
                    }
                    int count = 0;
                    for (uint idx = 0; idx < new_edge_list[another_user].size(); idx++)
                    {
                        uint now_item = new_edge_list[another_user][idx].first;
                        if (now_item == item_neighbor || now_item == item)
                        {
                            // if (new_edge_list[another_user][idx].second == 0)
                            // {
                            //     std::cout << "Error Support Computation" << "\n"; 
                            // }
                            new_edge_list[another_user][idx].second --;
                            if (new_edge_list[another_user][idx].second == 0)
                            {
                                std::vector<uint>::iterator remove_it = std::remove(
                                    item_neighbor_list[now_item].begin(),
                                    item_neighbor_list[now_item].end(),
                                    another_user
                                );
                                item_neighbor_list[now_item].resize(remove_it - item_neighbor_list[now_item].begin());
                            }
                            count ++;
                        }
                        if (count == 2) break;
                    }
                }
            }
            // new_edge_list[user].erase(new_edge_list[user].begin()+item_idx);
            // if (new_edge_list[user][item_idx].second != 0)
            // {
            //     std::cout << "Error Support Computation" << "\n"; 
            // }
        }
        // (2) clear drop edge list and detect all unqualified edge
        drop_edge_num = drop_edge_list.size();
    }
    Print_Time_Now("Filter Support: ", start);

    std::set<uint> user_set;
    std::set<uint> item_set;
    std::vector<std::pair<uint, uint>> new_e_list;
    for (size_t i=0; i<new_edge_list.size();i++)
    {
        for (auto edge : new_edge_list[i])
        {
            if (edge.second > 0)
            {
                user_set.insert(i);
                item_set.insert(edge.first);
                new_e_list.emplace_back(std::pair(i, edge.first));
            }
        }
    }

    new_e_list.shrink_to_fit();
    std::vector<uint> user_list;
    user_list.assign(user_set.begin(), user_set.end());
    std::vector<uint> item_list;
    item_list.assign(item_set.begin(), item_set.end());
    return new InducedGraph(this->graph, user_list, item_list, new_e_list);
}


/// @brief obtain a (K, r, \sigma)-bitruss from current (k,r)-bitruss subgraph
/// @param sigma 
/// @return InducedGraph type: a maximal (K, r, \sigma)-bitruss of current subgraph
InducedGraph* InducedGraph::ComputeSigmaBitruss(uint sigma)
{
    // 0.use adjacency list to save the edges
    std::vector<std::vector<std::pair<uint, uint>>> user_relationship_score_list(this->user_map.back()+1);

    std::vector<std::vector<uint>> user_neighbor_list(this->user_map.back()+1);
    
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
    for (size_t i = 0; i< this->user_map.size(); i++)
    {
        uint user1 = this->user_map[i];
        for (size_t j = i+1;j<this->user_map.size();j++)
        {
            // 1.1. for the choosed two item, find common neighbor users
            uint user2 = this->user_map[j];
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
            if ((sum*sum-square_sum)/2 < sigma)
            {
                return new InducedGraph(this->graph, {}, {}, {});
            }
            user_relationship_score_list[user1].emplace_back(
                std::pair(user2, (sum*sum-square_sum)/2)
            );
            user_relationship_score_list[user2].emplace_back(
                std::pair(user1, (sum*sum-square_sum)/2)
            );
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

    return new InducedGraph(
        this->graph,
        this->user_map,
        this->item_map,
        this->e_lists
    );
}


uint InducedGraph::CompareScore(uint user1, uint user2, std::vector<std::vector<uint>> user_relationship_score_list)
{
    uint sum1 = 0;
    std::for_each(
        user_relationship_score_list[user1].begin(),
        user_relationship_score_list[user1].end(),
        [&sum1](uint info){
            sum1 += info;
        }
    );
    uint sum2 = 0;
    std::for_each(
        user_relationship_score_list[user2].begin(),
        user_relationship_score_list[user2].end(),
        [&sum2](uint info){
            sum2 += info;
        }
    );
    if (sum1 > sum2) return user2;
    else if (sum1 < sum2) return user1;
    else return std::min(user1, user2);
}


uint InducedGraph::CompareScore(uint user1, uint user2, std::vector<std::vector<std::pair<uint, uint>>>& user_relationship_score_list)
{
    uint sum1 = 0;
    std::for_each(
        user_relationship_score_list[user1].begin(),
        user_relationship_score_list[user1].end(),
        [&sum1](std::pair<uint, uint> info){
            sum1 += info.second;
        }
    );
    uint sum2 = 0;
    std::for_each(
        user_relationship_score_list[user2].begin(),
        user_relationship_score_list[user2].end(),
        [&sum2](std::pair<uint, uint> info){
            sum2 += info.second;
        }
    );
    if (sum1 > sum2) return user2;
    else if (sum1 < sum2) return user1;
    else return std::min(user1, user2);
}

InducedGraph* InducedGraph::ComputeKRSigmaBitrussSimple(uint k, uint sigma, float& data_compute_time, float& edge_filter_time)
{
     // 0. use adjacency list to save the edges
    std::vector<std::vector<std::pair<uint, uint>>> new_edge_list(this->user_map.back()+1);
    std::vector<std::vector<uint>> item_neighbor_list(this->item_map.back()+1);

    std::vector<std::vector<std::pair<uint, uint>>> user_relationship_score_list(this->user_map.back()+1);
    std::vector<std::vector<uint>> user_neighbor_list(this->user_map.back()+1);
    
   for (auto edge : this->e_lists)
   {
       new_edge_list[edge.first].emplace_back(std::pair{edge.second, 0});
       item_neighbor_list[edge.second].emplace_back(edge.first);
       user_neighbor_list[edge.first].emplace_back(edge.second);
   }

    std::chrono::high_resolution_clock::time_point start = Get_Time();

    // 2. compute the score 
    // uint count = 0;
    for (size_t i = 0; i< this->user_map.size(); i++)
    {
        uint user1 = this->user_map[i];
        for (size_t j = i+1;j<this->user_map.size();j++)
        {
            // 2.1. for the choosed two item, find common neighbor users
            uint user2 = this->user_map[j];


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

            uint sum = 0, square_sum = 0;
            for (uint common_item : raw_common_neighbors)
            {
                // 2.2. compute the support
                uint sup_increment = raw_common_neighbors.size() - 1;
                // if (sup_increment > 0)
                //     count ++;
                for (auto& edge_info: new_edge_list[user1])
                    if (edge_info.first == common_item)
                        edge_info.second += sup_increment;
                for (auto& edge_info: new_edge_list[user2])
                    if (edge_info.first == common_item)
                        edge_info.second += sup_increment;
                
                // 2.3. compute the user relationship score
                uint wedge_score = std::min(
                    this->graph.GetEdgeData(user1, common_item)->weight,
                    this->graph.GetEdgeData(user2, common_item)->weight
                );
                sum += wedge_score;
                square_sum += wedge_score*wedge_score;
            }
            user_relationship_score_list[user1].emplace_back(
                std::pair(user2, (sum*sum-square_sum)/2)
            );
            user_relationship_score_list[user2].emplace_back(
                std::pair(user1, (sum*sum-square_sum)/2)
            );
            // user_relationship_score_list[user1][user2] = (sum*sum-square_sum)/2;
            // user_relationship_score_list[user2][user1] = (sum*sum-square_sum)/2;
        }
    }
    // std::cout << "Support Count: " << count << std::endl;

    float compute_time = Duration(start);
    Print_Time("Score Compute: ", compute_time);
    data_compute_time += compute_time;


    // 3. compute the (k,r,sigma)-bitruss
    start = Get_Time();
    uint drop_edge_num = 1;
    while(drop_edge_num > 0)
    {
        // std::cout << "Dropping edges: " << drop_edge_num << std::endl;
        drop_edge_num = 0;
        std::vector<std::pair<uint, uint>> drop_edge_list;
        
        // 3.1. detect low support edge
        for (uint user: this->user_map)
        {
            for (size_t idx=0; idx<new_edge_list[user].size();idx++)
            {
                if (new_edge_list[user][idx].second > 0 && new_edge_list[user][idx].second < k)
                    drop_edge_list.emplace_back(std::pair(user, idx));
            }
        }
        drop_edge_list.shrink_to_fit();
        drop_edge_num = drop_edge_list.size();
        // 3.2. check score only no edge dropped by sup
        if (drop_edge_num == 0)
        {
            // detect low score user
            for (uint user: this->user_map)
            {
                for (size_t idx=0; idx<user_relationship_score_list[user].size();idx++)
                {

                    if (user_relationship_score_list[user][idx].second > 0 &&
                    user_relationship_score_list[user][idx].second < sigma)
                    {
                        uint n_user = user_relationship_score_list[user][idx].first;
                        uint delete_user = CompareScore(
                            user, 
                            n_user,
                            user_relationship_score_list
                        );
                        uint another_user = user;
                        if (delete_user == user) another_user = n_user;
                        std::vector<uint> raw_common_neighbors(
                            user_neighbor_list[another_user].size() +
                            user_neighbor_list[delete_user].size()
                        );
                        std::vector<uint>::iterator it = std::set_intersection(
                            user_neighbor_list[another_user].begin(),
                            user_neighbor_list[another_user].end(),
                            user_neighbor_list[delete_user].begin(),
                            user_neighbor_list[delete_user].end(),
                            raw_common_neighbors.begin()
                        );
                        raw_common_neighbors.resize(it - raw_common_neighbors.begin());
                        for (uint item_idx=0; item_idx<new_edge_list[delete_user].size();item_idx++)
                        {
                            if (std::binary_search(
                                raw_common_neighbors.begin(),
                                raw_common_neighbors.end(),
                                new_edge_list[delete_user][item_idx].first
                            )) drop_edge_list.emplace_back(std::pair(delete_user, item_idx));
                        }
                    }
                }
            }
            drop_edge_list.shrink_to_fit();

            // std::cout << "Drop Edge Count:" << drop_edge_list.size() << std::endl;
            // std::set<std::pair<uint,uint>>pair_set(drop_edge_list.begin(), drop_edge_list.end());
            // std::cout << "Unique Drop Edge Count:" << pair_set.size() << std::endl;

            drop_edge_num = drop_edge_list.size();
        }

        // 3.3. remove the detection edges and update the related edges
        for (auto drop_edge_info : drop_edge_list)
        {
            // (1) extract the drop edge info
            uint user = drop_edge_info.first;
            uint item_idx = drop_edge_info.second;
            if (new_edge_list[user][item_idx].second == 0)
                continue;
            uint item = new_edge_list[user][item_idx].first;
            // (2) find butterflies containing drop_edge
            for (size_t i = 0; i< new_edge_list[user].size(); i++)
            {
                if (new_edge_list[user][item_idx].second == 0) break;
                if (new_edge_list[user][i].second == 0) continue;
                uint item_neighbor = new_edge_list[user][i].first;
                if (item_neighbor == item) continue;

                std::vector<uint> common_neighbors(
                    item_neighbor_list[item].size() +
                    item_neighbor_list[item_neighbor].size()
                );
                std::vector<uint>::iterator it = std::set_intersection(
                    item_neighbor_list[item].begin(),
                    item_neighbor_list[item].end(),
                    item_neighbor_list[item_neighbor].begin(),
                    item_neighbor_list[item_neighbor].end(),
                    common_neighbors.begin()
                );
                common_neighbors.resize(it - common_neighbors.begin());

                if (common_neighbors.size() < 2) continue;
                for (uint another_user : common_neighbors)
                {
                    // (3) maintain the sup info
                    if (another_user == user) continue;
                    if (new_edge_list[user][item_idx].second == 0)
                    {
                        std::cout << "Error Support Computation" << "\n"; 
                    }
                    new_edge_list[user][item_idx].second --;
                    if (new_edge_list[user][item_idx].second == 0)
                    {
                        std::vector<uint>::iterator remove_it = std::remove(
                            item_neighbor_list[item].begin(),
                            item_neighbor_list[item].end(),
                            user
                        );
                        item_neighbor_list[item].resize(remove_it - item_neighbor_list[item].begin());
                        remove_it = std::remove(
                            user_neighbor_list[user].begin(),
                            user_neighbor_list[user].end(),
                            item
                        );
                        user_neighbor_list[user].resize(remove_it - user_neighbor_list[user].begin());
                    }
                    if (new_edge_list[user][i].second == 0)
                    {
                        std::cout << "Error Support Computation" << "\n"; 
                    }
                    new_edge_list[user][i].second --;
                    if (new_edge_list[user][i].second == 0)
                    {
                        std::vector<uint>::iterator remove_it = std::remove(
                            item_neighbor_list[item_neighbor].begin(),
                            item_neighbor_list[item_neighbor].end(),
                            user
                        );
                        item_neighbor_list[item_neighbor].resize(remove_it - item_neighbor_list[item_neighbor].begin());
                        remove_it = std::remove(
                            user_neighbor_list[user].begin(),
                            user_neighbor_list[user].end(),
                            item_neighbor
                        );
                        user_neighbor_list[user].resize(remove_it - user_neighbor_list[user].begin());
                    }
                    int count = 0;
                    for (uint idx = 0; idx < new_edge_list[another_user].size(); idx++)
                    {
                        uint now_item = new_edge_list[another_user][idx].first;
                        if (now_item == item_neighbor || now_item == item)
                        {
                            if (new_edge_list[another_user][idx].second == 0)
                            {
                                std::cout << "Error Support Computation" << "\n"; 
                            }
                            new_edge_list[another_user][idx].second --;
                            if (new_edge_list[another_user][idx].second == 0)
                            {
                                std::vector<uint>::iterator remove_it = std::remove(
                                    item_neighbor_list[now_item].begin(),
                                    item_neighbor_list[now_item].end(),
                                    another_user
                                );
                                item_neighbor_list[now_item].resize(remove_it - item_neighbor_list[now_item].begin());
                                remove_it = std::remove(
                                    user_neighbor_list[another_user].begin(),
                                    user_neighbor_list[another_user].end(),
                                    now_item
                                );
                                user_neighbor_list[another_user].resize(remove_it - user_neighbor_list[another_user].begin());
                            }
                            count ++;
                        }
                        if (count == 2) break;
                    }

                    // (4) maintain the score
                    uint now_butterfly_score = std::min(
                        this->graph.GetEdgeData(user, item)->weight,
                        this->graph.GetEdgeData(another_user, item)->weight
                    ) * std::min(
                        this->graph.GetEdgeData(user, item_neighbor)->weight,
                        this->graph.GetEdgeData(another_user, item_neighbor)->weight
                    );
                    for (uint idx = 0; idx < user_relationship_score_list[user].size(); idx++)
                    {
                        uint now_user = user_relationship_score_list[user][idx].first;
                        if (now_user == another_user)
                        {
                            user_relationship_score_list[user][idx].second -= now_butterfly_score;
                            break;
                        }
                    }
                    for (uint idx = 0; idx < user_relationship_score_list[another_user].size(); idx++)
                    {
                        uint now_user = user_relationship_score_list[another_user][idx].first;
                        if (now_user == user)
                        {
                            user_relationship_score_list[another_user][idx].second -= now_butterfly_score;
                            break;
                        }
                    }
                }
            }
            if (new_edge_list[user][item_idx].second != 0)
            {
                std::cout << "Error Support Computation" << "\n"; 
            }
        }

    }
    float filter_time = Duration(start);
    Print_Time("Edge Filter: ", filter_time);
    edge_filter_time += filter_time;

    std::set<uint> user_set;
    std::set<uint> item_set;
    std::vector<std::pair<uint, uint>> new_e_list;
    for (uint i=0; i<new_edge_list.size();i++)
    {
        for (auto edge : new_edge_list[i])
        {
            if (edge.second > 0)
            {
                user_set.insert(i);
                item_set.insert(edge.first);
                new_e_list.emplace_back(std::pair(i, edge.first));
            }
        }
    }
    new_e_list.shrink_to_fit();
    std::vector<uint> user_list;
    user_list.assign(user_set.begin(), user_set.end());
    std::vector<uint> item_list;
    item_list.assign(item_set.begin(), item_set.end());
    return new InducedGraph(
        this->graph,
        user_list,
        item_list,
        new_e_list
    );
}


uint InducedGraph::CompareScore(uint user1, uint user2, std::vector<std::vector<UserData*>>& local_user_neighbor_data)
{   
    uint sum1 = 0;
    std::for_each(
        local_user_neighbor_data[user1].begin(),
        local_user_neighbor_data[user1].end(),
        [&sum1](UserData* info){
            sum1 += (info->x_data*info->x_data - info->y_data)/2;
        }
    );
    uint sum2 = 0;
    std::for_each(
        local_user_neighbor_data[user2].begin(),
        local_user_neighbor_data[user2].end(),
        [&sum2](UserData* info){
            sum2 += (info->x_data*info->x_data - info->y_data)/2;
        }
    );
    if (sum1 > sum2) return user2;
    else if (sum1 < sum2) return user1;
    else return std::min(user1, user2);
}

uint InducedGraph::FindIdxInUserDataList(uint user, uint another_user, std::vector<std::vector<UserData *>> local_user_neighbor_data)
{
    for (size_t idx=0; idx<local_user_neighbor_data[user].size();idx++)
    {
        if (local_user_neighbor_data[user][idx]->user_id == another_user)
        {
            return idx;
        }
    }
    return NOT_EXIST;
}


void InducedGraph::RemoveItemFromUserDataList(
    uint user,
    uint item,
    std::vector<uint>& reachable_user_list,
    std::vector<std::vector<UserData *>> local_user_neighbor_data
)
{
    // auto lower = std::lower_bound(
    //     local_user_neighbor_data[user][another_user_idx]->wedge_item_list.begin(),
    //     local_user_neighbor_data[user][another_user_idx]->wedge_item_list.end(),
    //     item
    // );

    // size_t dis = std::distance(local_user_neighbor_data[user][another_user_idx]->wedge_item_list.begin(), lower);

    // if (
    //     lower != local_user_neighbor_data[user][another_user_idx]->wedge_item_list.end() && *lower == item)
    // {
    //     local_user_neighbor_data[user][another_user_idx]->wedge_item_list.erase(
    //         local_user_neighbor_data[user][another_user_idx]->wedge_item_list.begin() + dis);
    //     local_user_neighbor_data[user][another_user_idx]->wedge_score_list.erase(
    //         local_user_neighbor_data[user][another_user_idx]->wedge_score_list.begin() + dis);

            
    //     local_user_neighbor_data[another_user][user_idx]->wedge_item_list.erase(
    //         local_user_neighbor_data[another_user][user_idx]->wedge_item_list.begin() + dis);
    //     local_user_neighbor_data[another_user][user_idx]->wedge_score_list.erase(
    //         local_user_neighbor_data[another_user][user_idx]->wedge_score_list.begin() + dis);
    // }

    for (uint another_user: reachable_user_list)
    {
        // (1) delete local_user_neighbor_data[user][another_user_idx]
        uint another_user_idx = FindIdxInUserDataList(user, another_user, local_user_neighbor_data);
        for (uint item_idx=0;item_idx < local_user_neighbor_data[user][another_user_idx]->wedge_item_list.size();item_idx++)
        {
            if (local_user_neighbor_data[user][another_user_idx]->wedge_item_list[item_idx] == item)
            {
                local_user_neighbor_data[user][another_user_idx]->wedge_item_list[item_idx] = NOT_EXIST;
                local_user_neighbor_data[user][another_user_idx]->wedge_score_list[item_idx] = 0;
                // local_user_neighbor_data[user][another_user_idx]->wedge_item_list.erase(
                //     local_user_neighbor_data[user][another_user_idx]->wedge_item_list.begin() + item_idx);
                // local_user_neighbor_data[user][another_user_idx]->wedge_score_list.erase(
                //     local_user_neighbor_data[user][another_user_idx]->wedge_score_list.begin() + item_idx);
                break;
            }
        }
        // (2) delete local_user_neighbor_data[another_user][user_idx]
        uint user_idx = FindIdxInUserDataList(another_user, user, local_user_neighbor_data);
        for (uint item_idx=0;item_idx < local_user_neighbor_data[another_user][user_idx]->wedge_item_list.size();item_idx++)
        {
            if (local_user_neighbor_data[another_user][user_idx]->wedge_item_list[item_idx] == item)
            {
                local_user_neighbor_data[another_user][user_idx]->wedge_item_list[item_idx] = NOT_EXIST;
                local_user_neighbor_data[another_user][user_idx]->wedge_score_list[item_idx] = 0;
                // local_user_neighbor_data[another_user][user_idx]->wedge_item_list.erase(
                //     local_user_neighbor_data[another_user][user_idx]->wedge_item_list.begin() + item_idx);
                // local_user_neighbor_data[another_user][user_idx]->wedge_score_list.erase(
                //     local_user_neighbor_data[another_user][user_idx]->wedge_score_list.begin() + item_idx);
                break;
            }
        }
    }
}

InducedGraph* InducedGraph::ComputeKRSigmaBitruss(uint k, uint sigma, float& data_compute_time, float& edge_filter_time)
{
     // 0. use adjacency list to save the edges
    std::vector<std::vector<std::pair<uint, uint>>> new_edge_list(this->user_map.back()+1);
    std::vector<std::vector<uint>> item_neighbor_list(this->item_map.back()+1);
    std::vector<std::vector<std::pair<uint, uint>>> user_relationship_score_list(this->user_map.back()+1);
    std::vector<std::vector<uint>> user_neighbor_list(this->user_map.back()+1);


    for (auto edge : this->e_lists)
    {
        new_edge_list[edge.first].emplace_back(std::pair{edge.second, 0});
        item_neighbor_list[edge.second].emplace_back(edge.first);
        user_neighbor_list[edge.first].emplace_back(edge.second);
    }

    std::chrono::high_resolution_clock::time_point start = Get_Time();

    // 1. compute the support and score
    // uint count = 0;
    for (uint user1: this->user_map)
    {
        for (WedgeData* user2_data: this->user_neighbor_datas[user1])
        {
            uint user2 = user2_data->user_id;
            if (user1 >= user2) continue;

            uint sup_increment = user2_data->wedge_item_list.size() - 1;
            if (sup_increment > 0)
            {
                // count += sup_increment;
                uint sum = 0, square_sum = 0;
                for (uint item_idx=0; item_idx<user2_data->wedge_item_list.size();item_idx++)
                {
                    // 1.1. compute the support                    
                    for (auto& edge_info: new_edge_list[user1])
                        if (edge_info.first == user2_data->wedge_item_list[item_idx])
                            edge_info.second += sup_increment;
                    for (auto& edge_info: new_edge_list[user2])
                        if (edge_info.first == user2_data->wedge_item_list[item_idx])
                            edge_info.second += sup_increment;
                    // 1.2. compute the score  
                    uint wedge_score = std::min(
                        this->graph.GetEdgeData(user1, user2_data->wedge_item_list[item_idx])->weight,
                        this->graph.GetEdgeData(user2, user2_data->wedge_item_list[item_idx])->weight
                    );
                    sum += wedge_score;
                    square_sum += wedge_score*wedge_score;
                }

                user_relationship_score_list[user1].emplace_back(
                    std::pair(user2, (sum*sum-square_sum)/2)
                );
                user_relationship_score_list[user2].emplace_back(
                    std::pair(user1, (sum*sum-square_sum)/2)
                );
            }
        }
    }
    // std::cout << "Support Count: " << count << std::endl;

    float compute_time = Duration(start);
    Print_Time("Score Compute: ", compute_time);
    data_compute_time += compute_time;

    // 2. compute the (k,r,sigma)-bitruss
    start = Get_Time();
    uint drop_edge_num = 1;
    while(drop_edge_num > 0)
    {
        // std::cout << "Dropping edges: " << drop_edge_num << std::endl;
        drop_edge_num = 0;
        std::vector<std::pair<uint, uint>> drop_edge_list;
        
        // 2.1. detect low support edge
        for (uint user: this->user_map)
        {
            for (size_t idx=0; idx<new_edge_list[user].size();idx++)
            {
                if (new_edge_list[user][idx].second > 0 && new_edge_list[user][idx].second < k)
                    drop_edge_list.emplace_back(user, idx);
            }
        }
        drop_edge_list.shrink_to_fit();
        drop_edge_num = drop_edge_list.size();

        // 2.2. check score only no edge dropped by sup
        if (drop_edge_num == 0)
        {
            // uint count = 0;
            for (uint user: this->user_map)
            {
                for (size_t idx=0; idx<user_relationship_score_list[user].size();idx++)
                {
                    if (user >= user_relationship_score_list[user][idx].first) continue;
                    // std::cout << user << " " << idx << " " << user_relationship_score_list[user][idx].first << " " << user_relationship_score_list[user][idx].second << std::endl;
                    // if (user_relationship_score_list[user][idx].second > 0)
                    // {
                    //     count += user_relationship_score_list[user][idx].second;
                    // }
                    if (user_relationship_score_list[user][idx].second > 0 &&
                    user_relationship_score_list[user][idx].second < sigma)
                    {
                        // 2.2.2. find the user to drop
                        uint n_user = user_relationship_score_list[user][idx].first;
                        uint delete_user = CompareScore(
                            user, 
                            n_user,
                            user_relationship_score_list
                        );
                        uint another_user = user;
                        if (delete_user == user) another_user = n_user;
                        // 2.2.3. find the edges need to drop
                        std::vector<uint> raw_common_neighbors(
                            user_neighbor_list[another_user].size() +
                            user_neighbor_list[delete_user].size()
                        );
                        std::vector<uint>::iterator it = std::set_intersection(
                            user_neighbor_list[another_user].begin(),
                            user_neighbor_list[another_user].end(),
                            user_neighbor_list[delete_user].begin(),
                            user_neighbor_list[delete_user].end(),
                            raw_common_neighbors.begin()
                        );
                        raw_common_neighbors.resize(it - raw_common_neighbors.begin());
                        for (uint item_idx=0; item_idx<new_edge_list[delete_user].size();item_idx++)
                        {
                            if (std::binary_search(
                                raw_common_neighbors.begin(),
                                raw_common_neighbors.end(),
                                new_edge_list[delete_user][item_idx].first
                            )) drop_edge_list.emplace_back(delete_user, item_idx);
                        }
                    }
                }
            }
            // std::cout << "Count:" << count << std::endl;
            drop_edge_list.shrink_to_fit();

            // std::cout << "Drop Edge Count:" << drop_edge_list.size() << std::endl;
            // std::set<std::pair<uint,uint>>pair_set(drop_edge_list.begin(), drop_edge_list.end());
            // std::cout << "Unique Drop Edge Count:" << pair_set.size() << std::endl;

            drop_edge_num = drop_edge_list.size();
        }

        // 2.3. remove the detection edges and update the related edges
        for (auto drop_edge_info : drop_edge_list)
        {
            // (1) extract the drop edge info
            uint user = drop_edge_info.first;
            uint item_idx = drop_edge_info.second;
            if (new_edge_list[user][item_idx].second == 0)
                continue;
            uint item = new_edge_list[user][item_idx].first;
            // (2) find butterflies containing drop_edge
            for (size_t i = 0; i< new_edge_list[user].size(); i++)
            {
                if (new_edge_list[user][item_idx].second == 0) break;
                if (new_edge_list[user][i].second == 0) continue;
                uint item_neighbor = new_edge_list[user][i].first;
                if (item_neighbor == item) continue;

                std::vector<uint> common_neighbors(
                    item_neighbor_list[item].size() +
                    item_neighbor_list[item_neighbor].size()
                );
                std::vector<uint>::iterator it = std::set_intersection(
                    item_neighbor_list[item].begin(),
                    item_neighbor_list[item].end(),
                    item_neighbor_list[item_neighbor].begin(),
                    item_neighbor_list[item_neighbor].end(),
                    common_neighbors.begin()
                );
                common_neighbors.resize(it - common_neighbors.begin());

                if (common_neighbors.size() < 2) continue;
                // (3) for each common neighbor, maintain the sup info and score
                for (uint another_user : common_neighbors)
                {
                    if (another_user == user) continue;
                    if (new_edge_list[user][item_idx].second == 0)
                    {
                        std::cout << "Error Support Computation1" << "\n"; 
                    }

                    // maintain sup
                    new_edge_list[user][item_idx].second --;
                    if (new_edge_list[user][item_idx].second == 0)
                    {
                        std::vector<uint>::iterator remove_it = std::remove(
                            item_neighbor_list[item].begin(),
                            item_neighbor_list[item].end(),
                            user
                        );
                        item_neighbor_list[item].resize(remove_it - item_neighbor_list[item].begin());
                        remove_it = std::remove(
                            user_neighbor_list[user].begin(),
                            user_neighbor_list[user].end(),
                            item
                        );
                        user_neighbor_list[user].resize(remove_it - user_neighbor_list[user].begin());
                    }
                    if (new_edge_list[user][i].second == 0)
                    {
                        std::cout << "Error Support Computation2" << "\n"; 
                    }
                    // maintain sup
                    new_edge_list[user][i].second --;
                    if (new_edge_list[user][i].second == 0)
                    {
                        std::vector<uint>::iterator remove_it = std::remove(
                            item_neighbor_list[item_neighbor].begin(),
                            item_neighbor_list[item_neighbor].end(),
                            user
                        );
                        item_neighbor_list[item_neighbor].resize(remove_it - item_neighbor_list[item_neighbor].begin());
                        remove_it = std::remove(
                            user_neighbor_list[user].begin(),
                            user_neighbor_list[user].end(),
                            item_neighbor
                        );
                        user_neighbor_list[user].resize(remove_it - user_neighbor_list[user].begin());
                    }
                    int count = 0;
                    for (uint idx = 0; idx < new_edge_list[another_user].size(); idx++)
                    {
                        uint now_item = new_edge_list[another_user][idx].first;
                        if (now_item == item_neighbor || now_item == item)
                        {
                            if (new_edge_list[another_user][idx].second == 0)
                            {
                                std::cout << "Error Support Computation3" << "\n"; 
                            }
                            new_edge_list[another_user][idx].second --;
                            if (new_edge_list[another_user][idx].second == 0)
                            {
                                std::vector<uint>::iterator remove_it = std::remove(
                                    item_neighbor_list[now_item].begin(),
                                    item_neighbor_list[now_item].end(),
                                    another_user
                                );
                                item_neighbor_list[now_item].resize(remove_it - item_neighbor_list[now_item].begin());
                                remove_it = std::remove(
                                    user_neighbor_list[another_user].begin(),
                                    user_neighbor_list[another_user].end(),
                                    now_item
                                );
                                user_neighbor_list[another_user].resize(remove_it - user_neighbor_list[another_user].begin());
                            }
                            count ++;
                        }
                        if (count == 2) break;
                    }

                    // (4) maintain the score
                    uint now_butterfly_score = std::min(
                        this->graph.GetEdgeData(user, item)->weight,
                        this->graph.GetEdgeData(another_user, item)->weight
                    ) * std::min(
                        this->graph.GetEdgeData(user, item_neighbor)->weight,
                        this->graph.GetEdgeData(another_user, item_neighbor)->weight
                    );
                    for (uint idx = 0; idx < user_relationship_score_list[user].size(); idx++)
                    {
                        // uint now_user = user_relationship_score_list[user][idx].first;
                        if (user_relationship_score_list[user][idx].first == another_user)
                        {
                            user_relationship_score_list[user][idx].second -= now_butterfly_score;
                            break;
                        }
                    }
                    for (uint idx = 0; idx < user_relationship_score_list[another_user].size(); idx++)
                    {
                        // uint now_user = user_relationship_score_list[another_user][idx].first;
                        if (user_relationship_score_list[another_user][idx].first == user)
                        {
                            user_relationship_score_list[another_user][idx].second -= now_butterfly_score;
                            break;
                        }
                    }
                }
            }
            if (new_edge_list[user][item_idx].second != 0)
            {
                std::cout << "Error Support Computation4: rest [" <<new_edge_list[user][item_idx].second  << "]" << "\n"; 
            }
        }

    }
    float filter_time = Duration(start);
    Print_Time("Edge Filter: ", filter_time);
    edge_filter_time += filter_time;


    std::set<uint> user_set;
    std::set<uint> item_set;
    std::vector<std::pair<uint, uint>> new_e_list;
    for (uint i=0; i<new_edge_list.size();i++)
    {
        for (auto edge : new_edge_list[i])
        {
            if (edge.second > 0)
            {
                user_set.insert(i);
                item_set.insert(edge.first);
                new_e_list.emplace_back(std::pair(i, edge.first));
            }
        }
    }
    new_e_list.shrink_to_fit();
    std::vector<uint> user_list;
    user_list.assign(user_set.begin(), user_set.end());
    std::vector<uint> item_list;
    item_list.assign(item_set.begin(), item_set.end());

    InducedGraph* result_graph = nullptr;
    try {
        result_graph = new InducedGraph(
            this->graph,
            user_list,
            item_list,
            new_e_list
        );
    } catch (const std::bad_alloc& e) {
        std::cout << "Memory allocation failed: " << e.what() << std::endl;
    }
    return result_graph;
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
    result_str += "Edge List[" + std::to_string(e_lists.size()) +  "]: ";
    for(auto edge: e_lists)
    {
        result_str += ("(" + std::to_string(edge.first) + "," + std::to_string(edge.second) + ") ");
    }
    result_str += "\n";
    return result_str;
}


std::string InducedGraph::PrintShortMetaData()
{
    std::string result_str;
    result_str += "U[" + std::to_string(user_map.size()) +  "]";
    result_str += " ";
    result_str += "I[" + std::to_string(item_map.size()) +  "]";
    result_str += " ";
    result_str += "E[" + std::to_string(e_lists.size()) +  "]";
    result_str += " ";
    return result_str;
}