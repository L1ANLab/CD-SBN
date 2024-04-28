#include <algorithm>
#include <cmath>
#include <vector>

#include "utils/types.h"
#include "graph/graph.h"
#include "graph/induced_graph.h"

InducedGraph::InducedGraph()
: graph()
, user_map{}
, item_map{}
, e_lists{}
{}

InducedGraph::InducedGraph(const Graph& g, uint user1, uint user2, uint item1, uint item2)
: graph(&g)
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

uint InducedGraph::GetDegree(uint v)
{
    uint degree = 0u;

    for (auto& e: e_lists)
        if (e.first == v || e.second == v)
            degree++;

    return degree;
}