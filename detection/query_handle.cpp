#include "detection/query_handle.h"

bool HeapEntry::operator < (const HeapEntry &right) const
{
    if (this->score == right.score)
    {
    return this->node->GetLevel() < right.node->GetLevel();
    }
    
    return this->score < right.score; 
}

bool HeapEntry::operator > (const HeapEntry &right) const
{
    if (this->score == right.score)
    {
        return this->node->GetLevel() > right.node->GetLevel();
    }
    
    return this->score > right.score; 
}

bool CompareHeapEntry(const HeapEntry *left, const HeapEntry *right)
{
    return *left < *right;
}


QueryHandle::QueryHandle(
    std::vector<uint> query_keywords_,
    uint query_support_threshold_,
    uint query_radius_,
    uint query_score_threshold_,
    Graph* data_graph_,
    Synopsis* syn_
)
: query_keywords(query_keywords_)
, query_support_threshold(query_support_threshold_)
, query_radius(query_radius_)
, query_score_threshold(query_score_threshold_)
, data_graph(data_graph_)
, syn(syn_)
{
    // hash the query keywords set to query_BV
    for (uint keyword : this->query_keywords)
    {
        this->query_BV.set(keyword);
    }
    
    // transform the query radius
    this->query_radius_idx = this->query_radius - 1;
}


QueryHandle::~QueryHandle() {}


bool QueryHandle::CheckPruningConditions(SynopsisNode* node)
{
    // (Index-Level) Keyword Pruning
    if ((node->GetBvR(this->query_radius_idx) & this->query_BV) == 0)
        return false;
    // (Index-Level) Support Pruning
    if (node->GetUbSupM(this->query_radius_idx) < query_support_threshold)
        return false;
    // (Index-Level) User Relationship Score Pruning
    if (node->GetUbScore(this->query_radius_idx) < query_score_threshold)
        return false;
    return true;
}

/// @brief check whether the <to_insert_community> can be inserted into <candidate_set> 
/// @param candidate_set 
/// @param to_insert_community 
/// @return true if can be inserted, otherwise false
bool QueryHandle::CheckCommunityInsert(
    std::set<InducedGraph*>& candidate_set,
    InducedGraph* to_insert_community
)
{
    std::set<InducedGraph*>::iterator iter = candidate_set.begin();
    while(iter!=candidate_set.end())
    {
        // (1) compute the common users
        std::vector<uint> common_users(
            (*iter)->user_map.size() + to_insert_community->user_map.size()
        );
        std::vector<uint>::iterator it = std::set_intersection(
            (*iter)->user_map.begin(), (*iter)->user_map.end(),
            to_insert_community->user_map.begin(), to_insert_community->user_map.end(),
            common_users.begin()
        );
        uint common_user_num = it - common_users.begin();
        // (2) discuss conditions
        if ((*iter)->user_map.size() < to_insert_community->user_map.size())
        {   // if *iter is smaller and is a subset
            if ((*iter)->user_map.size() == common_user_num)
            {   //  (*iter) is a subset
                candidate_set.erase(iter++);
                continue;
            }
        }
        else
        {
            if (to_insert_community->user_map.size() == common_user_num)
            {
                return false;
            }
        }
        iter++;
    }

    return true;
}
