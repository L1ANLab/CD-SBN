#include <vector>
#include <algorithm>
#include <iostream>
#include <cmath>

#include "utils/types.h"
#include "detection/synopsis.h"

#pragma region SynopsisNode

uint SynopsisNode::ID_COUNTER = 0;

/// @brief TreeNode & LeafNode construction
/// @param children_entries_ 
SynopsisNode::SynopsisNode(
    uint level_,
    std::vector<SynopsisNode*>& children_entries_
):id(ID_COUNTER), level(level_)
{
    children_entries = children_entries_;
    for (int r=0;r<R_MAX;r++)
    {
        SynopsisData* data_r = new SynopsisData();
        for(size_t i=0;i<children_entries_.size();i++)
        {
            data_r->bv_r |= children_entries_[i]->data[r]->bv_r;
            data_r->ub_sup_M = std::max(
                data_r->ub_sup_M,
                children_entries_[i]->data[r]->ub_sup_M
            );
            data_r->ub_score = std::max(
                data_r->ub_score,
                children_entries_[i]->data[r]->ub_score
            );
            std::merge(
                this->user_set.begin(), this->user_set.end(),
                children_entries_[i]->user_set.begin(), children_entries_[i]->user_set.end(),
                this->user_set.begin()
            );
        }
    }

    std::vector<SynopsisNode*>().swap(children_entries_);
    ID_COUNTER ++;
}

/// @brief Vertex entry construction
/// @param bv_r_ 
/// @param ub_sup_M_ 
/// @param ub_score_ 
/// @param vertex_entry 
SynopsisNode::SynopsisNode(
    uint level_,
    SynopsisData* data_[],
    uint user_id
):id(ID_COUNTER), level(level_), user_set{user_id}
, children_entries(0)
{
    for (int r=0;r<R_MAX;r++)
    {
        data[r] = data_[r];
    }
    ID_COUNTER ++;
}

SynopsisNode::~SynopsisNode()
{
    for (size_t i=0; i<R_MAX;i++)
    {
        delete data[i];
    }
    std::vector<SynopsisNode*>().swap(children_entries);
    std::vector<uint>().swap(user_set);
}

uint SynopsisNode::GetID() { return id; }

void SynopsisNode::SetBvR(std::bitset<MAX_LABEL> bv_r_, uint r) { data[r]->bv_r = bv_r_; }
std::bitset<MAX_LABEL> SynopsisNode::GetBvR(uint r) const { return data[r]->bv_r; }

void SynopsisNode::SetUbSupM(uint ub_sup_M_, uint r) { data[r]->ub_sup_M = ub_sup_M_; }
uint SynopsisNode::GetUbSupM(uint r) const { return data[r]->ub_sup_M; }

void SynopsisNode::SetUbScore(uint ub_score_, uint r) { data[r]->ub_score = ub_score_; }
uint SynopsisNode::GetUbScore(uint r) const { return data[r]->ub_score; }
SynopsisData* SynopsisNode::GetSynopsisData(uint r) const { return data[r]; }
uint SynopsisNode::GetLevel() const { return level; }

std::vector<uint> SynopsisNode::GetUserSet() const { return user_set; }

const std::vector<SynopsisNode*>& SynopsisNode::GetChildren() const { return children_entries; }
bool SynopsisNode::IsTreeNode() const { return (children_entries.size() < user_set.size());}
bool SynopsisNode::IsLeafNode() const { return (children_entries.size() == user_set.size()); }
bool SynopsisNode::IsVertexEntry() const { return (children_entries.size() == 0 && user_set.size() > 1); }

#pragma endregion SynopsisNode


#pragma region Synopsis
// Synopsis

Synopsis::Synopsis(): root(nullptr), inv_list{}
{}

Synopsis::~Synopsis()
{
    DestroySynopsis(root);
}

SynopsisNode* Synopsis::GetRoot() const { return root; }


/// @brief build a synopsis $Syn$ of graph
/// @param graph 
/// @return a pointer to the root node of synopsis
SynopsisNode* Synopsis::BuildSynopsis(Graph* graph)
{
    // 0. initialize a inverted list to store the node index for each vertex
    inv_list.resize(graph->UserVerticesNum());

    // 1. package the vertex into synopsis vertex entry
    std::vector<SynopsisNode*> vertex_entry_list(0);
    for (size_t i=0;i<graph->UserVerticesNum();i++)
    {
        SynopsisNode* node_pointer = CreateVertexEntry(i, graph);
        vertex_entry_list.push_back(node_pointer);
        inv_list[i].emplace_back(node_pointer);
    }
    // 2. sort the synopsis nodes
    std::sort(
        vertex_entry_list.begin(),
        vertex_entry_list.end(),
        [](SynopsisNode* left, SynopsisNode* right)
        {
            uint left_score = 0;
            uint right_score = 0;
            for (int r=0;r<R_MAX;r++)
            {
                left_score += left->GetUbScore(r);
                right_score += right->GetUbScore(r);
            }

            return left_score > right_score;
        }
    );
    // 3. recusively call function to split and merge the synopsis nodes
    SynopsisNode* root = BuildSynopsisRecursively(vertex_entry_list, 0);
    // 4. return the root node pointer
    return root;
}


/// @brief update the synopsis upon the edge
/// @param user_id the id of user vertex
/// @param item_id the id of item vertex
/// @param addition_flag 1 if new edge was added, else 0
/// @param graph the graph
/// @return 
bool Synopsis::UpdateSynopsisAfterInsertion(uint user_id, uint item_id, uint addition_flag, Graph* graph)
{
    EdgeData* inserted_edge = graph->GetEdgeData(user_id, item_id);
    const std::vector<uint> item_v_a_neighbor_list = graph->GetItemNeighbors(item_id);
    const std::vector<uint> user_u_x_neighbor_list = graph->GetUserNeighbors(user_id);

    // 0. insert the new user into synopsis node
    if (inv_list.size() <= user_id) inv_list.resize(user_id + 1);

    if (inv_list[user_id].empty()) 
    {
        SynopsisNode* node_pointer = CreateVertexEntry(user_id, graph);
        inv_list[user_id].emplace_back(node_pointer);
        InsertVertexEntry(user_id, node_pointer);
    }

    std::bitset<MAX_LABEL> u_i_BV = graph->GetUserBv(user_id);
    // 1. for all possible radii r
    for (int r=0; r<R_MAX; r++)
    {
        auto [affected_user_list, affected_item_list] = graph->Get2rHopOfUser(user_id, r);
        // 2. for each affected vertex
        for (auto affected_user_id : affected_user_list)
        {
            auto [affected_user_2r_list, affected_item_2r_list] = graph->Get2rHopOfUser(affected_user_id, r);
            // 3. for each affected nodes (from bottom to top)
            for (auto affected_node : inv_list[user_id])
            {
                bool changed_flag = false;
                // compute BV and sup if addition
                if (addition_flag == 1)
                {
                    // 3.1. compute BV_r
                    std::bitset<MAX_LABEL> node_BV_r = affected_node->GetBvR(r);
                    std::bitset<MAX_LABEL> new_BV_r = affected_node->GetBvR(r) | u_i_BV;
                    if (node_BV_r != new_BV_r)
                    {
                        affected_node->SetBvR(new_BV_r, r);
                        changed_flag = true;
                    }

                    // 3.2. compute ub_sup_M
                    uint new_max_sup = inserted_edge->ub_sup;
                    for(size_t i = 0;i < item_v_a_neighbor_list.size();i++)
                    {
                        uint n_user_id = item_v_a_neighbor_list[i];
                        if (n_user_id == user_id) continue;  // skip the user of <user_id>

                        if (!std::binary_search(affected_user_2r_list.begin(), affected_user_2r_list.end(), n_user_id))
                            continue;

                        std::vector<uint> common_neighbors(
                            user_u_x_neighbor_list.size() + user_u_x_neighbor_list.size()
                        );
                        std::vector<uint> user_u_n_neighbor_list = graph->GetUserNeighbors(n_user_id);
                        std::vector<uint>::iterator it = std::set_intersection(
                            user_u_x_neighbor_list.begin(), user_u_x_neighbor_list.end(),
                            user_u_n_neighbor_list.begin(), user_u_n_neighbor_list.end(),
                            common_neighbors.begin()
                        );
                        common_neighbors.resize(it - common_neighbors.begin());

                        new_max_sup = std::max(new_max_sup, graph->GetEdgeData(n_user_id, item_id)->ub_sup);

                        for(size_t j = 0; j < common_neighbors.size(); j++)
                        {
                            uint cn_item_id = common_neighbors[j];
                            if (cn_item_id == item_id) continue;
                            new_max_sup = std::max(new_max_sup, graph->GetEdgeData(user_id, cn_item_id)->ub_sup);
                            new_max_sup = std::max(new_max_sup, graph->GetEdgeData(n_user_id, cn_item_id)->ub_sup);
                        }
                    }
                    if (affected_node->GetUbSupM(r) < new_max_sup)
                    {
                        affected_node->SetUbSupM(new_max_sup, r);       
                        changed_flag = true;
                    }
                }
                // 3.3. re-compute ub_score_M
                uint new_max_ub_score = 0;

                std::vector<UserData*> neighbor_datas = graph->GetNeighborUserData(user_id);
                for (UserData* user_data: neighbor_datas)
                {
                    if (!std::binary_search(affected_user_2r_list.begin(), affected_user_2r_list.end(), user_data->user_id))
                        continue; // skip if the user neighbor of <user_id> is not contained in the 2r-hop.
                    uint now_score = (user_data->x_data*user_data->x_data + user_data->y_data)/2;
                    new_max_ub_score = std::max(new_max_ub_score, now_score);
                }
                
                if (affected_node->GetUbScore(r) < new_max_ub_score)
                {
                    affected_node->SetUbScore(new_max_ub_score, r);
                    changed_flag = true;
                }

                if (!changed_flag) break; // break if no data is changed
            }
        }
    }

    return true;
}

/// @brief update the synopsis upon the edge
/// @param user_id the id of user vertex
/// @param item_id the id of item vertex
/// @param addition_flag 1 if new edge was removed, else 0
/// @param graph the graph
/// @return 
bool Synopsis::UpdateSynopsisAfterExpiration(uint user_id, uint item_id, uint removal_flag, Graph* graph)
{
    // EdgeData* inserted_edge = graph->GetEdgeData(user_id, item_id);
    const std::vector<uint> item_v_a_neighbor_list = graph->GetItemNeighbors(item_id);
    const std::vector<uint> user_u_x_neighbor_list = graph->GetUserNeighbors(user_id);

    // 0. insert the new user into synopsis node
    if (inv_list.size() <= user_id) inv_list.resize(user_id + 1);

    // TODO: Handle the deletion condition
    // if (inv_list[user_id].empty()) 
    // {
    //     SynopsisNode* node_pointer = CreateVertexEntry(user_id, graph);
    //     inv_list[user_id].emplace_back(node_pointer);
    //     InsertVertexEntry(user_id, node_pointer);
    // }

    // std::bitset<MAX_LABEL> u_i_BV = graph->GetUserBv(user_id);
    // 1. for all possible radii r
    for (int r=0; r<R_MAX; r++)
    {
        auto [affected_user_list, affected_item_list] = graph->Get2rHopOfUser(user_id, r);
        // 2. for each affected vertex
        for (auto affected_user_id : affected_user_list)
        {
            auto [affected_user_2r_list, affected_item_2r_list] = graph->Get2rHopOfUser(affected_user_id, r);
            // 3. for each affected nodes (from bottom to top)
            for (auto affected_node : inv_list[user_id])
            {
                bool changed_flag = false;
                // compute BV and sup if addition
                if (removal_flag == 1)
                {
                    // 3.1. compute BV_r
                    std::bitset<MAX_LABEL> node_BV_r = affected_node->GetBvR(r);
                    std::bitset<MAX_LABEL> new_BV_r;
                    for(uint affect_hop_user_id: affected_user_2r_list)
                    {
                        new_BV_r |= graph->GetUserBv(affect_hop_user_id);
                    }
                    if (node_BV_r != new_BV_r)
                    {
                        affected_node->SetBvR(new_BV_r, r);
                        changed_flag = true;
                    }

                    // 3.2. compute ub_sup_M
                    uint new_max_sup = 0;
                    for (uint affect_hop_user_id: affected_user_2r_list)
                    {
                        auto user_neighbor_item_list = graph->GetUserNeighbors(affect_hop_user_id);
                        for (uint neighbor_item_id: user_neighbor_item_list)
                        {
                            EdgeData* edge_data = graph->GetEdgeData(affect_hop_user_id, neighbor_item_id);
                            if (edge_data != nullptr)
                            {
                                new_max_sup = std::max(new_max_sup, edge_data->ub_sup);
                            }
                        }
                    }
                    if (affected_node->GetUbSupM(r) < new_max_sup)
                    {
                        affected_node->SetUbSupM(new_max_sup, r);       
                        changed_flag = true;
                    }
                }
                // 3.3. re-compute ub_score_M
                uint new_max_ub_score = 0;
                for (uint affect_hop_user_id: affected_user_2r_list)
                {
                    std::vector<UserData*> neighbor_datas = graph->GetNeighborUserData(affect_hop_user_id);
                    for (UserData* user_data: neighbor_datas)
                    {
                        if (!std::binary_search(affected_user_2r_list.begin(), affected_user_2r_list.end(), user_data->user_id))
                            continue; // skip if the user neighbor of <user_id> is not contained in the 2r-hop.
                        uint now_score = (user_data->x_data*user_data->x_data + user_data->y_data)/2;
                        new_max_ub_score = std::max(new_max_ub_score, now_score);
                    }
                }
                
                if (affected_node->GetUbScore(r) < new_max_ub_score)
                {
                    affected_node->SetUbScore(new_max_ub_score, r);
                    changed_flag = true;
                }

                if (!changed_flag) break; // break if no data is changed
            }
        }
    }

    return true;
}


/// @brief [Private] create a vertex entry for the new user
/// @param user_id 
/// @param graph 
/// @return a vertex entry of the <user_id> with aggregates
SynopsisNode* Synopsis::CreateVertexEntry(uint user_id, Graph* graph)
{
    SynopsisData* data_[R_MAX];
    for (int r=0; r<R_MAX; r++)
    {
        auto [user_list, item_list] = graph->Get2rHopOfUser(user_id, r);
        // 0.1. compute BV_r
        std::bitset<MAX_LABEL> bv_r_;
        for(uint hop_user_id: user_list)
        {
            bv_r_ |= graph->GetUserBv(hop_user_id);
        }
        uint ub_sup_M_ = 0;
        // 0.2. compute ub_sup_M
        for (uint hop_user_id: user_list)
        {
            auto user_neighbor_item_list = graph->GetUserNeighbors(hop_user_id);
            for (uint hop_item_id: user_neighbor_item_list)
            {
                EdgeData* edge_data = graph->GetEdgeData(hop_user_id, hop_item_id);
                if (edge_data != nullptr)
                {
                    ub_sup_M_ = std::max(ub_sup_M_, edge_data->ub_sup);
                }
            }
        }
        // 0.3. compute ub_score_M
        uint ub_score = 0;
        for (uint hop_user_id: user_list)
        {
            std::vector<UserData*> neighbor_datas = graph->GetNeighborUserData(hop_user_id);
            for (UserData* user_data: neighbor_datas)
            {
                if (!std::binary_search(user_list.begin(), user_list.end(), user_data->user_id))
                    continue; // skip if the user neighbor of <user_id> is not contained in the 2r-hop.
                uint now_score = (user_data->x_data*user_data->x_data + user_data->y_data)/2;
                ub_score = std::max(ub_score, now_score);
            }
        }
        // 0.4. package a synopsis node 
        data_[r]->bv_r = bv_r_;
        data_[r]->ub_sup_M = ub_sup_M_;
        data_[r]->ub_score = ub_score;
    }
    return new SynopsisNode(MAX_LEVEL, data_, user_id);
}

/// @brief [Private] set the children of each synopsis node and build the synopsis tree
/// @param vertex_entry_list 
/// @param level 
/// @return a pointer to the root node of synopsis
SynopsisNode* Synopsis::BuildSynopsisRecursively(
    std::vector<SynopsisNode*> vertex_entry_list,
    uint level
)
{
    if (vertex_entry_list.size() <= SYNOPSIS_SIZE)
    {
        SynopsisNode* leaf_node_pointer = new SynopsisNode(level, vertex_entry_list);
        for (auto user_id: leaf_node_pointer->GetUserSet())
        {
            inv_list[user_id].emplace_back(leaf_node_pointer);
        }
        return leaf_node_pointer;
    }

    size_t partition_num = ceil(vertex_entry_list.size() / SYNOPSIS_SIZE);
    std::vector<SynopsisNode*> children_entries_;
    for (size_t i=0;i<partition_num;i++)
    {
        size_t start_idx = i*partition_num;
        size_t end_idx = std::min(i*partition_num + partition_num, vertex_entry_list.size());
        std::vector<SynopsisNode*> partition_vertex_entry_list(
            vertex_entry_list.begin()+start_idx,
            vertex_entry_list.begin()+end_idx
        );
        SynopsisNode* child_entry = BuildSynopsisRecursively(partition_vertex_entry_list, level+1);
        children_entries_.push_back(child_entry);
    }
    children_entries_.resize(children_entries_.size());

    SynopsisNode* non_leaf_node_pointer = new SynopsisNode(level, vertex_entry_list);
    for (auto user_id: non_leaf_node_pointer->GetUserSet())
    {
        inv_list[user_id].emplace_back(non_leaf_node_pointer);
    }
    return non_leaf_node_pointer;
}


/// @brief [Private] traverse the synopsis to insert a new vertex entry
/// @param new_vertex_entry
void Synopsis::InsertVertexEntry(uint user_id, SynopsisNode* new_vertex_entry)
{
    
    uint new_vertex_entry_score = 0;
    for (int r=0;r<R_MAX;r++)
    {
        new_vertex_entry_score += new_vertex_entry->GetUbScore(r);
    }
    // SynopsisNode* now_node_pointer = root;
    SearchSynopsisTrace(user_id, root, new_vertex_entry_score);
}


/// @brief [Private] search a leaf node to insert a new vertex
/// @param now_node_pointer 
/// @param inv_list_element 
/// @param new_vertex_entry_score 
void Synopsis::SearchSynopsisTrace(uint user_id, SynopsisNode* now_node_pointer, uint new_vertex_entry_score)
{
    std::vector<SynopsisNode*> children_list = now_node_pointer->GetChildren();

    if (now_node_pointer->GetLevel() == MAX_LEVEL)
    {
        return;
    }

    auto child_iter = std::lower_bound(children_list.begin(), children_list.end(), new_vertex_entry_score,
        [](const SynopsisNode* child, uint new_vertex_entry_score)
        {
            uint child_score = 0;
            for (int r=0;r<R_MAX;r++)
            {
                child_score += child->GetUbScore(r);
            }
            return child_score > new_vertex_entry_score;
        }
    );
    if (child_iter == children_list.end()) child_iter = children_list.end() - 1;
    inv_list[user_id].emplace_back(*child_iter);
    SearchSynopsisTrace(user_id, *child_iter, new_vertex_entry_score);
    return;
}

void Synopsis::DestroySynopsis(SynopsisNode* now_node_pointer)
{
    if (now_node_pointer != nullptr) {
        for (SynopsisNode* child : now_node_pointer->GetChildren())
        {
            DestroySynopsis(child);
        }
        delete now_node_pointer;
    }
}
#pragma endregion Synopsis