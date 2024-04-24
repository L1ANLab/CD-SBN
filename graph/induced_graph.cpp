#include <algorithm>
#include <cmath>
#include <vector>

#include "utils/types.h"
#include "graph/graph.h"
#include "graph/induced_graph.h"

InducedGraph::InducedGraph()
: graph()
, user_map_{}
, item_map_{}
, e_lists_{}
{}

InducedGraph::InducedGraph(const Graph& g, uint user1, uint user2, uint item1, uint item2)
: graph(&g)
, user_map_{std::min(user1, user2), std::max(user1, user2)}
, item_map_{std::min(item1, item2), std::max(item1, item2)}
, e_lists_{{user1, item1}, {user1, item2}, {user2, item1}, {user2, item2}}
{}

InducedGraph::InducedGraph(InducedGraph& g1, InducedGraph& g2, bool is_union)
: graph(g1.graph)
, user_map_(g1.user_map_.size() + g2.user_map_.size())
, item_map_(g1.item_map_.size() + g2.item_map_.size())
, e_lists_(g1.e_lists_.size() + g2.e_lists_.size())
{
    if (is_union)
    {
        std::vector<uint>::iterator it1 = std::set_union(
            g1.user_map_.begin(), g1.user_map_.end(),
            g2.user_map_.begin(), g2.user_map_.end(),
            user_map_.begin()
        );
        user_map_.resize(it1 - user_map_.begin());

        std::vector<uint>::iterator it2 = std::set_union(
            g1.item_map_.begin(), g1.item_map_.end(),
            g2.item_map_.begin(), g2.item_map_.end(),
            item_map_.begin()
        );
        item_map_.resize(it2 - item_map_.begin());

        std::vector<std::pair<uint, uint>>::iterator it3 = std::set_union(
            g1.e_lists_.begin(), g1.e_lists_.end(),
            g2.e_lists_.begin(), g2.e_lists_.end(),
            e_lists_.begin()
        );
        e_lists_.resize(it3 - e_lists_.begin());
    }
    else
    {
        std::vector<uint>::iterator it1 = std::set_intersection(
            g1.user_map_.begin(), g1.user_map_.end(),
            g2.user_map_.begin(), g2.user_map_.end(),
            user_map_.begin()
        );
        user_map_.resize(it1 - user_map_.begin());

        std::vector<uint>::iterator it2 = std::set_intersection(
            g1.item_map_.begin(), g1.item_map_.end(),
            g2.item_map_.begin(), g2.item_map_.end(),
            item_map_.begin()
        );
        item_map_.resize(it2 - item_map_.begin());

        std::vector<std::pair<uint, uint>>::iterator it3 = std::set_intersection(
            g1.e_lists_.begin(), g1.e_lists_.end(),
            g2.e_lists_.begin(), g2.e_lists_.end(),
            e_lists_.begin()
        );
        e_lists_.resize(it3 - e_lists_.begin());
    }
}

uint InducedGraph::GetDegree(uint v)
{
    uint degree = 0u;

    for (auto& e: e_lists_)
        if (e.first == v || e.second == v)
            degree++;

    return degree;
}