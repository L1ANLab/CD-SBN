
#ifndef BASELINEHANDLE_H
#define BASELINEHANDLE_H

#include "detection/query_handle.h"

class BaselineHandle: public QueryHandle
{
private:

public:
    BaselineHandle(
        std::vector<uint> query_keywords_,
        uint query_support_threshold_,
        uint query_radius_,
        uint query_score_threshold_,
        Graph* graph_,
        Synopsis* syn_
    );
    ~BaselineHandle();

    std::vector<InducedGraph*> ExecuteQuery(
        Statistic* stat
    );
};


#endif // BASELINEHANDLE_H 