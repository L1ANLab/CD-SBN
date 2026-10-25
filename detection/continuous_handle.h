#ifndef CONTINUOUSHANDLE_H
#define CONTINUOUSHANDLE_H

#include "detection/query_handle.h"

// Wall times are in milliseconds. A batch publishes one final result set.
struct ContinuousBatchStats
{
    size_t updates = 0;
    size_t expirations = 0;
    size_t candidate_centers = 0;
    size_t recomputed_centers = 0;
    size_t invalidated_communities = 0;
    float result_maintain_ms = 0;
    float total_ms = 0;
};

class ContinuousHandle: public QueryHandle
{
private:
    struct BatchContext
    {
        std::vector<uint> updated_users;
        std::vector<uint> candidate_centers;
        std::vector<bool> invalid_results;
        size_t invalidated_communities = 0;
        size_t recomputed_centers = 0;
    };
    BatchContext PrepareBatch(
        const std::vector<uint>& updated_users,
        const std::vector<InducedGraph*>& result_list);
    uint ExecuteBatchQuery(
        Statistic* stat, std::vector<InducedGraph*>& result_list, BatchContext& batch);

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

    // stream[begin,end) must be the next consecutive records after the current
    // graph state. window_begin is advanced once for each expired record.
    // capacity counts records (including repeats), not distinct graph edges.
    // result_list must be the complete answer on the pre-batch graph; initialize
    // it with SnapshotHandle before the first call.
    // The caller owns result_list; affected graphs are replaced by this call.
    ContinuousBatchStats ExecuteBatch(
        Statistic* stat,
        std::vector<InducedGraph*>& result_list,
        std::vector<SynopsisNode*>& vertex_entry_list,
        const std::vector<InsertUnit>& stream,
        size_t begin, size_t end,
        size_t& window_begin, size_t capacity);

    uint ExecuteQuery(
        Statistic* stat,
        std::vector<InducedGraph*>& result_list,
        std::vector<SynopsisNode*>& vertex_entry_list,
        uint isRemoved, uint expire_edge_user_id, uint expire_edge_item_id,
        uint insert_edge_user_id, std::vector<uint> insert_related_user_list
    );
};


#endif // CONTINUOUSHANDLE_H 