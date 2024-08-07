
#ifndef QUERYHANDLE_H
#define QUERYHANDLE_H

#include <set>
#include "graph/induced_graph.h"
#include "detection/synopsis.h"
#include "utils/statistic.h"

struct HeapEntry
{
    SynopsisNode* node;
    uint score;

    HeapEntry(SynopsisNode* node_, uint score_): node(node_), score(score_) {}
    bool operator < (const HeapEntry &right) const;
    bool operator > (const HeapEntry &right) const;
};

bool CompareHeapEntry(const HeapEntry *left, const HeapEntry *right);

class QueryHandle
{
protected:
    /* data */
    std::vector<uint> query_keywords;
    uint query_support_threshold;
    uint query_radius;
    uint query_score_threshold;
    Graph* data_graph;
    Synopsis* syn;

    std::bitset<MAX_LABEL>  query_BV;
    uint query_radius_idx;
    
    bool CheckPruningConditions(SynopsisNode* node);
    bool CheckCommunityInsert(
        std::set<std::unique_ptr<InducedGraph>>& candidate_set,
        const std::unique_ptr<InducedGraph>& to_insert_community
    );

public:
    QueryHandle(
        std::vector<uint> query_keywords_,
        uint query_support_threshold_,
        uint query_radius_,
        uint query_score_threshold_,
        Graph* graph_,
        Synopsis* syn_
    );
    ~QueryHandle();
};


#endif // QUERYHANDLE_H 