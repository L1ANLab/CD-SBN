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

    std::vector<std::unique_ptr<InducedGraph>> ExecuteQuery(
        Statistic* stat,
        std::vector<std::unique_ptr<InducedGraph>> result_list,
        uint isRemoved, uint expire_edge_user_id, uint expire_edge_item_id,
        uint insert_edge_user_id, std::vector<uint> insert_related_user_list
    );
};


#endif // CONTINUOUSHANDLE_H 