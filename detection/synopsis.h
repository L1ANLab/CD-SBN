#ifndef CDSBN_SYNOPSIS_H
#define CDSBN_SYNOPSIS_H

#include <vector>
#include <unordered_set>
#include <bitset>

#include "graph/graph.h"

struct SynopsisData
{
    std::shared_ptr<std::bitset<MAX_LABEL>> bv_r;
    uint ub_sup_M;
    uint ub_score;

    SynopsisData(): bv_r{}, ub_sup_M(0), ub_score(0){}

    SynopsisData(
        std::shared_ptr<std::bitset<MAX_LABEL>> bv_r_,
        uint ub_sup_M_,
        uint ub_score_
    ): ub_sup_M(ub_sup_M_)
    , ub_score(ub_score_)
    {
        bv_r = std::move(bv_r_);
    }
};

class SynopsisNode
{
protected:
    static uint ID_COUNTER;

    uint id;
    uint level;
    std::shared_ptr<SynopsisData> data[R_MAX];
    std::vector<uint> user_set;

    std::vector<SynopsisNode*> children_entries;
public:
    SynopsisNode(
        uint level_,
        std::vector<SynopsisNode*>& children_entries_
    ); // TreeNode & LeafNode construct 
    SynopsisNode(
        uint level_,
        std::shared_ptr<SynopsisData> data_[],
        uint user_id
    ); // Vertex entry construct
    ~SynopsisNode();

    uint GetID();
    void SetBvR(std::shared_ptr<std::bitset<MAX_LABEL>>& bv_r_, uint r);
    const std::shared_ptr<std::bitset<MAX_LABEL>>& GetBvR(uint r) const;
    void SetUbSupM(uint ub_sup_M_, uint r);
    uint GetUbSupM(uint r) const;
    void SetUbScore(uint ub_score_, uint r);
    uint GetUbScore(uint r) const;
    // std::unique_ptr<SynopsisData> GetSynopsisData(uint r) const;
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

    bool PrecomputeSynopsisEntries(Graph* graph, std::vector<SynopsisNode*>& vertex_entry_list);
    bool LoadSynopsisEntries(
        std::string synopsis_file_path,
        std::vector<SynopsisNode*>& vertex_entry_list
    );
    SynopsisNode* BuildSynopsis(Graph* graph, std::vector<SynopsisNode*>& vertex_entry_list);

    bool SaveSynopsisEntries(std::string synopsis_file_path, std::vector<SynopsisNode*> vertex_entry_list);

    bool UpdateSynopsisAfterInsertion(uint user_id, uint item_id, uint addition_flag, Graph* Graph);
    bool UpdateSynopsisAfterExpiration(uint user_id, uint item_id, uint removal_flag, Graph* Graph);

    uint GetInvListSize();
    std::vector<SynopsisNode*> GetInvListByUser(uint user_id);
};

#endif //CDSBN_SYNOPSIS_H