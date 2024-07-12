#ifndef CDSBN_SYNOPSIS_H
#define CDSBN_SYNOPSIS_H

#include <vector>
#include <unordered_set>
#include <bitset>

#include "graph/graph.h"

struct SynopsisData
{
    std::bitset<MAX_LABEL> bv_r;
    uint ub_sup_M;
    uint ub_score;

    SynopsisData(): bv_r{}, ub_sup_M(0), ub_score(0){}

    SynopsisData(
        std::bitset<MAX_LABEL> bv_r_,
        uint ub_sup_M_,
        uint ub_score_
    ): bv_r(bv_r_)
    , ub_sup_M(ub_sup_M_)
    , ub_score(ub_score_)
    {}
};

class SynopsisNode
{
protected:
    static uint ID_COUNTER;

    uint id;
    uint level;
    SynopsisData* data[R_MAX];
    std::vector<uint> user_set;

    std::vector<SynopsisNode*> children_entries;
public:
    SynopsisNode(
        uint level_,
        std::vector<SynopsisNode*>& children_entries_
    ); // TreeNode & LeafNode construct 
    SynopsisNode(
        uint level_,
        SynopsisData* data_[],
        uint user_id
    ); // Vertex entry construct
    ~SynopsisNode();

    uint GetID();
    void SetBvR(std::bitset<MAX_LABEL> bv_r_, uint r);
    std::bitset<MAX_LABEL> GetBvR(uint r) const;
    void SetUbSupM(uint ub_sup_M_, uint r);
    uint GetUbSupM(uint r) const;
    void SetUbScore(uint ub_score_, uint r);
    uint GetUbScore(uint r) const;
    SynopsisData* GetSynopsisData(uint r) const;
    uint GetLevel() const;
    std::vector<uint> GetUserSet() const;
    const std::vector<SynopsisNode*>& GetChildren() const;
    bool IsTreeNode() const;
    bool IsLeafNode() const;
    bool IsVertexEntry() const;
};


class Synopsis
{
private:
    SynopsisNode* root;
    std::vector<std::vector<SynopsisNode*>> inv_list;  
    // Store a list of synopsis pointer which contains the vertex whose ID is index (from bottom to top)

    SynopsisNode* CreateVertexEntry(uint user_id, Graph* graph);

    SynopsisNode* BuildSynopsisRecursively(
        std::vector<SynopsisNode*> vertex_entry_list,
        uint level
    );

    void InsertVertexEntry(uint user_id, SynopsisNode* new_vertex_entry);
    void SearchSynopsisTrace(uint user_id, SynopsisNode* now_node_pointer, uint new_vertex_entry_score);
    void DestroySynopsis(SynopsisNode* now_node_pointer);
    
public:
    Synopsis();
    ~Synopsis();

    SynopsisNode* GetRoot() const;
    uint CountLeafNodes(SynopsisNode* now_node) const;

    SynopsisNode* BuildSynopsis(Graph* graph);

    bool UpdateSynopsisAfterInsertion(uint user_id, uint item_id, uint addition_flag, Graph* Graph);

    bool UpdateSynopsisAfterExpiration(uint user_id, uint item_id, uint removal_flag, Graph* Graph);
};

#endif //CDSBN_SYNOPSIS_H