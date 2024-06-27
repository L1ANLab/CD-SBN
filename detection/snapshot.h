#ifndef SNAPSHOT_H
#define SNAPSHOT_H

#include "graph/induced_graph.h"
#include "detection/synopsis.h"
#include "utils/statistic.h"


class Snapshot
{
private:
    /* data */
    uint query_timestamp;
    std::vector<uint> query_keywords;
    uint query_support_threshold;
    uint query_radius;
    uint query_score_threshold;
    Graph* data_graph;
    Synopsis* syn;

    std::bitset<MAX_LABEL>  query_BV;
    uint query_radius_idx;
    
    bool CheckPruningConditions(SynopsisNode* node);

public:
    Snapshot(
        uint query_timestamp_,
        std::vector<uint> query_keywords_,
        uint query_support_threshold_,
        uint query_radius_,
        uint query_score_threshold_,
        Graph* graph_,
        Synopsis* syn_
    );
    ~Snapshot();

    std::vector<InducedGraph*> ExecuteSnapshotQuery(Statistic* stat);
};


#endif // SNAPSHOT_H 