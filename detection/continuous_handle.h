#ifndef CONTINUOUSHANDLE_H
#define CONTINUOUSHANDLE_H

#include "detection/query_handle.h"

class ContinuousHandle: public QueryHandle
{
private:

public:
    ContinuousHandle(
        std::vector<uint> query_keywords_,
        uint query_support_threshold_,
        uint query_radius_,
        uint query_score_threshold_,
        Graph* graph_,
        Synopsis* syn_
    );
    ~ContinuousHandle();

    std::vector<InducedGraph*> ExecuteQuery(Statistic* stat);
};


#endif // CONTINUOUSHANDLE_H 