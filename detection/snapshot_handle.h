#ifndef SNAPSHOTHANDLE_H
#define SNAPSHOTHANDLE_H

#include "detection/query_handle.h"

class SnapshotHandle: public QueryHandle
{
private:

public:
    SnapshotHandle(
        std::vector<uint> query_keywords_,
        uint query_support_threshold_,
        uint query_radius_,
        uint query_score_threshold_,
        Graph* graph_,
        Synopsis* syn_
    );
    ~SnapshotHandle();

    uint ExecuteQuery(Statistic* stat, std::vector<InducedGraph*>& result_list);
};


#endif // SNAPSHOTHANDLE_H 