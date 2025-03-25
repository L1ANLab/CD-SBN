#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cmath>
#include <iomanip>

#include "detection/synopsis.h"

// #pragma region SynopsisNode

uint SynopsisNode::ID_COUNTER = 0;

/// @brief TreeNode & LeafNode construction
/// @param children_entries_ 
SynopsisNode::SynopsisNode(
    uint level_,
    std::vector<SynopsisNode*>& children_entries_
):id(ID_COUNTER), level(level_), user_set{}
{
    this->children_entries.assign(children_entries_.begin(), children_entries_.end());
    
    for (uint r=0;r<R_MAX;r++)
    {
        std::shared_ptr<SynopsisData> data_r(new SynopsisData());
        data_r->bv_r.reset(new std::bitset<MAX_LABEL>(0));
        for(size_t i=0;i<children_entries_.size();i++)
        {
            *(data_r->bv_r) |= *(children_entries_[i]->data[r]->bv_r);
            data_r->ub_sup_M = std::max(
                data_r->ub_sup_M,
                children_entries_[i]->data[r]->ub_sup_M
            );
            data_r->ub_score = std::max(
                data_r->ub_score,
                children_entries_[i]->data[r]->ub_score
            );
        }
        this->data[r] = std::move(data_r);
    }
    std::vector<uint> temp_vec;
    for(size_t i=0;i<children_entries_.size();i++)
    {
        temp_vec.resize(this->user_set.size()+children_entries_[i]->user_set.size());
        std::merge(
            this->user_set.begin(), this->user_set.end(),
            children_entries_[i]->user_set.begin(), children_entries_[i]->user_set.end(),
            temp_vec.begin()
        );
        this->user_set.assign(temp_vec.begin(), temp_vec.end());
        std::vector<uint>().swap(temp_vec);
    }
    ID_COUNTER ++;
}

/// @brief Vertex entry construction
/// @param level_ 
/// @param data_ 
/// @param user_id 
SynopsisNode::SynopsisNode(
    uint level_,
    std::shared_ptr<SynopsisData> data_[],
    uint user_id
):id(ID_COUNTER), level(level_), user_set{user_id}
, children_entries(0)
{
    
    for (uint r=0;r<R_MAX;r++)
    {
        data[r] = std::move(data_[r]);
    }
    ID_COUNTER ++;
}


SynopsisNode::~SynopsisNode()
{
    // for (size_t i=0; i<R_MAX;i++)
    // {
    //     delete data[i];
    // }
    // std::vector<SynopsisNode*>().swap(children_entries);
    std::vector<uint>().swap(user_set);
}

uint SynopsisNode::GetID() { return id; }

void SynopsisNode::SetBvR(std::shared_ptr<std::bitset<MAX_LABEL>>& bv_r_, uint r) { data[r]->bv_r = std::move(bv_r_); }
const std::shared_ptr<std::bitset<MAX_LABEL>>& SynopsisNode::GetBvR(uint r) const { return data[r]->bv_r; }

void SynopsisNode::SetUbSupM(uint ub_sup_M_, uint r) { data[r]->ub_sup_M = ub_sup_M_; }
uint SynopsisNode::GetUbSupM(uint r) const { return data[r]->ub_sup_M; }

void SynopsisNode::SetUbScore(uint ub_score_, uint r) { data[r]->ub_score = ub_score_; }
uint SynopsisNode::GetUbScore(uint r) const { return data[r]->ub_score; }
// std::unique_ptr<SynopsisData> SynopsisNode::GetSynopsisData(uint r) const { return data[r]; }
uint SynopsisNode::GetLevel() const { return level; }

std::vector<uint> SynopsisNode::GetUserSet() const { return user_set; }

const std::vector<SynopsisNode*>& SynopsisNode::GetChildren() const { return children_entries; }
bool SynopsisNode::IsTreeNode() const { return (children_entries.size() < user_set.size());}
bool SynopsisNode::IsLeafNode() const { return (children_entries.size() == user_set.size()); }
bool SynopsisNode::IsVertexEntry() const { return (children_entries.size() == 0 && user_set.size() > 1); }

// #pragma endregion SynopsisNode


// #pragma region Synopsis
// Synopsis

Synopsis::Synopsis(): root(nullptr), inv_list{}
{}

Synopsis::~Synopsis()
{
    DestroySynopsis(root);
}

SynopsisNode* Synopsis::GetRoot() const { return root; }

uint Synopsis::CountLeafNodes(SynopsisNode* now_node) const
{
    if (now_node->GetChildren().size() == 0) return 0;
    if (now_node->GetChildren().front()->GetLevel() == MAX_LEVEL) return 1;
    uint now_counter = 0;
    for (SynopsisNode* child_node: now_node->GetChildren())
        now_counter += CountLeafNodes(child_node);
    return now_counter;
}



/// @brief Load synopsis entries from file
/// @param graph 
/// @return a vector of vertex entries
bool Synopsis::LoadSynopsisEntries(
    std::string synopsis_file_path,
    std::vector<SynopsisNode*>& vertex_entry_list
)
{
    ErrorControl::assert_error(
        !io::file_exists(synopsis_file_path.c_str()),
        "File Error: The input <" + synopsis_file_path  + "> file does not exists"
    );
    std::ifstream ifs(synopsis_file_path);
    ErrorControl::assert_error(
        !ifs,
        "File Stream Error: The input file stream open failed"
    );
    uint list_size;
    ifs >> list_size;
    // int counter = 0;
    this->inv_list.resize(list_size);
    while (!ifs.eof())
    {
        uint user_id;
        ifs >> user_id;
        std::shared_ptr<SynopsisData> data[R_MAX];
        for (uint r=0; r<R_MAX; r++)
        {
            std::string bv_str;
            uint ub_sup_M, ub_score;
            ifs >> bv_str >> ub_sup_M >> ub_score;
            std::shared_ptr<std::bitset<MAX_LABEL>> bv_r(new std::bitset<MAX_LABEL>(bv_str));
            std::shared_ptr<SynopsisData> data_r(new SynopsisData(std::move(bv_r), ub_sup_M, ub_score));
            data[r] = std::move(data_r);
        }
        
        SynopsisNode* node_pointer = new SynopsisNode(
            MAX_LEVEL,
            data,
            user_id
        );
        vertex_entry_list.emplace_back(node_pointer);
        this->inv_list[user_id].emplace_back(node_pointer);
    }
    ifs.close();
    return true;
}


/// @brief save the synopsis from root to file
/// @param synopsis_file_path 
/// @return true if file successfully saved, otherwise false
bool Synopsis::SaveSynopsisEntries(std::string synopsis_file_path, std::vector<SynopsisNode*> vertex_entry_list)
{
    ErrorControl::assert_error(
        io::file_exists(synopsis_file_path.c_str()),
        "File Error: The output <" + synopsis_file_path  + "> file exists"
    );
    std::ofstream of(synopsis_file_path.c_str(), std::ios::out);
    ErrorControl::assert_error(
        !of,
        "File Stream Error: The output file stream open failed"
    );

    of << vertex_entry_list.size() << std::endl;
    for (SynopsisNode* vertex_entry : vertex_entry_list)
    {
        of << vertex_entry->GetUserSet().front() << ' ';
        for (uint r=0; r<R_MAX; r++)
        {
            of << *(vertex_entry->GetBvR(r)) << ' ';
            of << vertex_entry->GetUbSupM(r) << ' ';
            of << vertex_entry->GetUbScore(r) << ' ';
        }
        of << std::endl;
    }
    of.close();
    return true;
}


/// @brief compute the vertex entries of synopsis.
/// @param graph 
/// @return a vector of vertex entries
bool Synopsis::PrecomputeSynopsisEntries(Graph* graph, std::vector<SynopsisNode*>& vertex_entry_list)
{
    // 0. initialize a inverted list to store the node index for each vertex
    inv_list.resize(graph->UserVerticesNum());
    // 1. package the vertex into synopsis vertex entry
    uint vertices_num = graph->UserVerticesNum();
    #pragma omp parallel for num_threads(THREADS_NUM)
    for (uint i=0;i<vertices_num;i++)
    {
        SynopsisNode* node_pointer = CreateVertexEntry(i, graph);
        inv_list[i].emplace_back(node_pointer);

        #pragma omp critical
        {
            vertex_entry_list.push_back(node_pointer);
            if (i%100 == 0)
            {
                std::cout << "\r" << i+1 << "/" << vertices_num;
                std::cout << std::fixed << std::setprecision(2) << " ("  << (i+1)*100.0/vertices_num << "%)" << std::endl;
            }
        }
    }
    std::cout << std::endl;
    return true;
}

/// @brief build a synopsis $syn$ of graph
/// @param graph 
/// @return a pointer to the root node of synopsis
SynopsisNode* Synopsis::BuildSynopsis(Graph* graph, std::vector<SynopsisNode*>& vertex_entry_list)
{
    // 1. recusively call function to split and merge the synopsis nodes
    this->root = BuildSynopsisRecursively(vertex_entry_list, 0);
    // 2. return the root node pointer
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
    std::shared_ptr<EdgeData> inserted_edge = graph->GetEdgeData(user_id, item_id);
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

    const std::shared_ptr<std::bitset<MAX_LABEL>>& u_i_BV = graph->GetUserBv(user_id);
    // 1. for all possible radii r
    for (uint r=0; r<R_MAX; r++)
    {
        uint radius = r+1;
        auto [affected_user_list, affected_item_list] = graph->Get2rHopOfUser(user_id, radius);
        // 2. for each affected vertex
        for (auto affected_user_id : affected_user_list)
        {
            auto [affected_user_2r_list, affected_item_2r_list] = graph->Get2rHopOfUser(affected_user_id, radius);
            // 3. for each affected nodes (from bottom to top)
            for (auto affected_node : inv_list[affected_user_id])
            {
                bool changed_flag = false;
                // compute BV and sup if addition
                if (addition_flag == 1)
                {
                    // 3.1. compute BV_r
                    std::shared_ptr<std::bitset<MAX_LABEL>> new_BV_r(new std::bitset<MAX_LABEL>(*(affected_node->GetBvR(r)) | *u_i_BV));
                    if (*(affected_node->GetBvR(r)) != *new_BV_r)
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

                const std::vector<UserData*>& neighbor_datas = graph->GetNeighborUserData(user_id);
                for (uint idx=0;idx < neighbor_datas.size(); idx++)
                {
                    if (!std::binary_search(
                        affected_user_2r_list.begin(),
                        affected_user_2r_list.end(),
                        neighbor_datas[idx]->user_id)
                    )
                        continue; // skip if the user neighbor of <user_id> is not contained in the 2r-hop.
                    uint now_score = (neighbor_datas[idx]->x_data*neighbor_datas[idx]->x_data +
                        neighbor_datas[idx]->y_data)/2;
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
    for (uint r=0; r<R_MAX; r++)
    {
        uint radius = r+1;
        auto [affected_user_list, affected_item_list] = graph->Get2rHopOfUser(user_id, radius);
        // 2. for each affected vertex
        for (auto affected_user_id : affected_user_list)
        {
            auto [affected_user_2r_list, affected_item_2r_list] = graph->Get2rHopOfUser(affected_user_id, radius);
            // 3. for each affected nodes (from bottom to top)
            for (auto affected_node : inv_list[affected_user_id])
            {
                bool changed_flag = false;
                // compute BV and sup if addition
                if (removal_flag == 1)
                {
                    // 3.1. compute BV_r
                    // std::bitset<MAX_LABEL> new_BV_r;
                    std::shared_ptr<std::bitset<MAX_LABEL>> new_BV_r(new std::bitset<MAX_LABEL>(0));
                    for(uint affect_hop_user_id: affected_user_2r_list)
                    {
                        *new_BV_r |= *(graph->GetUserBv(affect_hop_user_id));
                    }
                    if (*(affected_node->GetBvR(r)) != *new_BV_r)
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
                            std::shared_ptr<EdgeData> edge_data = graph->GetEdgeData(affect_hop_user_id, neighbor_item_id);
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
                    const std::vector<UserData*>& neighbor_datas = graph->GetNeighborUserData(affect_hop_user_id);
                    for (uint idx=0;idx<neighbor_datas.size();idx++)
                    {
                        if (!std::binary_search(
                            affected_user_2r_list.begin(),
                            affected_user_2r_list.end(),
                            neighbor_datas[idx]->user_id))
                            continue; // skip if the user neighbor of <user_id> is not contained in the 2r-hop.
                        uint now_score = (neighbor_datas[idx]->x_data*neighbor_datas[idx]->x_data +
                        neighbor_datas[idx]->y_data)/2;
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

uint Synopsis::GetInvListSize() { return this->inv_list.size(); }

std::vector<SynopsisNode*> Synopsis::GetInvListByUser(uint user_id) { return this->inv_list[user_id];}


/// @brief [Private] create a vertex entry for the new user
/// @param user_id 
/// @param graph 
/// @return a vertex entry of the <user_id> with aggregates
SynopsisNode* Synopsis::CreateVertexEntry(uint user_id, Graph* graph)
{
    std::shared_ptr<SynopsisData> data_[R_MAX];
    for (uint r=0; r<R_MAX; r++)
    {
        uint radius = r+1;
        auto [user_list, item_list] = graph->Get2rHopOfUser(user_id, radius);
        if (user_list.size() == 0) continue;
        // 0.1. compute BV_r
        std::shared_ptr<std::bitset<MAX_LABEL>> bv_r_(new std::bitset<MAX_LABEL>(0));
        for(uint hop_user_id: user_list)
        {
            *bv_r_ = *bv_r_ | *(graph->GetUserBv(hop_user_id));
        }
        // 0.2. compute ub_sup_M
        uint ub_sup_M_ = 0;
        for (uint hop_user_id: user_list)
        {
            auto user_neighbor_item_list = graph->GetUserNeighbors(hop_user_id);
            for (uint hop_item_id: user_neighbor_item_list)
            {
                const std::shared_ptr<EdgeData>& edge_data = graph->GetEdgeData(hop_user_id, hop_item_id);
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
            const std::vector<UserData*>& neighbor_datas = graph->GetNeighborUserData(hop_user_id);
            for (uint idx = 0; idx < neighbor_datas.size();idx++)
            {
                if (!std::binary_search(user_list.begin(), user_list.end(), neighbor_datas[idx]->user_id))
                    continue; // skip if the user neighbor of <user_id> is not contained in the 2r-hop.
                uint now_score = (neighbor_datas[idx]->x_data*neighbor_datas[idx]->x_data
                - neighbor_datas[idx]->y_data)/2;
                ub_score = std::max(ub_score, now_score);
            }
        }
        // 0.4. package a synopsis node
        std::shared_ptr<SynopsisData> data(new SynopsisData(std::move(bv_r_), ub_sup_M_, ub_score));
        data_[r] = std::move(data);
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

    size_t partition_entry_num = ceil(vertex_entry_list.size()*1.0 / SYNOPSIS_SIZE);
    std::vector<SynopsisNode*> children_entries_;
    for (size_t i=0;i<SYNOPSIS_SIZE;i++)
    {
        size_t start_idx = i*partition_entry_num;
        size_t end_idx = std::min((i+1)*partition_entry_num, vertex_entry_list.size());
        std::vector<SynopsisNode*> partition_vertex_entry_list(
            vertex_entry_list.begin() + start_idx,
            vertex_entry_list.begin() + end_idx
        );
        SynopsisNode* child_entry = BuildSynopsisRecursively(partition_vertex_entry_list, level+1);
        children_entries_.push_back(child_entry);
        if (end_idx == vertex_entry_list.size()) break;
    }
    children_entries_.shrink_to_fit();

    SynopsisNode* non_leaf_node_pointer = new SynopsisNode(level, children_entries_);
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
    for (uint r=0;r<R_MAX;r++)
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
    if (now_node_pointer->GetLevel() == MAX_LEVEL)
    {
        return;
    }

    std::vector<SynopsisNode*> children_list = now_node_pointer->GetChildren();

    auto child_iter = std::lower_bound(children_list.begin(), children_list.end(), new_vertex_entry_score,
        [](const SynopsisNode* child, uint new_vertex_entry_score)
        {
            uint child_score = 0;
            for (uint r=0;r<R_MAX;r++)
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

/// @brief [Private] recursively destroy the synopsis nodes from the pointer
/// @param now_node_pointer 
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





// # pragma endregion Synopsis