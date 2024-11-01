import os
from matplotlib import pyplot as plt
import networkx as nx
import numpy as np
from tqdm import tqdm
from itertools import product

R_MAX = 2


def compute_hop_v_r(graph: nx.Graph, node_v: int, radius: int) -> nx.Graph:
    nodes_in_hop = []
    to_traverse = [node_v]
    for r_dev in range(radius):
        new_to_traverse = []
        for node in to_traverse:
            for neighbor in graph.neighbors(node):
                if neighbor not in nodes_in_hop:
                    nodes_in_hop.append(neighbor)
                    new_to_traverse.append(neighbor)
        to_traverse = new_to_traverse
    return graph.subgraph(nodes_in_hop)


def load_graph_from_list(raw_data_list: list) -> nx.Graph:
    data_graph = nx.Graph()
    counter = 0
    data_list = []
    max_user = 0
    print("Process Graph Data")
    for line in tqdm(raw_data_list):
        if line.startswith("%"):
            continue
        line_list = line.strip().split()
        user = 0
        item = 0
        weight = 1
        tstamp = counter
        if len(line_list) == 2:
            [user, item] = line_list
        elif len(line_list) == 4:
            [user, item, weight, tstamp] = line_list
        weight = int(float(weight))
        tstamp = int(float(tstamp))
        data_list.append([int(user), int(item), weight, tstamp])
        if max_user < int(user):
            max_user = int(user)
        counter += 1

    print("Contruct NX Graph")
    for data in tqdm(data_list):
        if data_graph.has_edge(data[0], data[1]+max_user):
            data_graph.edges[data[0], data[1]+max_user]["weight"] += data[2]
        else:
            data_graph.add_edge(data[0], data[1]+max_user, weight=data[2], timestamp=data[3])
    return data_graph, max_user


def load_label_from_list(raw_data_list: list, max_user: int) -> dict:
    label_dict = dict()
    print("Process Label Data")
    for line in tqdm(raw_data_list):
        [item_id, labels] = line.rstrip().split(" ")
        label_dict[int(item_id)+max_user] = labels.split(",")
    return label_dict


def search_maximal_biclique(graph: nx.Graph, user_list: list, user_idx: int, neighbor_item_list: set):
    if user_idx == len(user_list):
        return []

    now_user_id = user_list[user_idx]
    now_user_neighbors = set(graph.neighbors(now_user_id))

    result1 = search_maximal_biclique(
        graph=graph,
        user_list=user_list,
        user_idx=user_idx+1,
        neighbor_item_list=(neighbor_item_list & now_user_neighbors)
    )

    result2 = search_maximal_biclique(
        graph=graph,
        user_list=user_list,
        user_idx=user_idx+1,
        neighbor_item_list=(neighbor_item_list)
    )

    if len(result1) > len(result2):
        return (neighbor_item_list & now_user_neighbors)
    else:
        return (neighbor_item_list)


if __name__ == "__main__":
    folder_path = "dataset"
    dataset_type = "realworld"  # baseline realworld synthetic ablation
    dir_name = "BS"
    full_path = os.path.join(os.getcwd(), folder_path, dataset_type, dir_name)
    print("------{}------".format(full_path))
    graph_file = "out.bibsonomy-2ui"
    label_file = "label_list.txt"
    label_name_file = "ent.bibsonomy-2ti.tag.name"

    # 0. read the graph
    raw_data_list = []
    with open(os.path.join(full_path, graph_file), encoding='utf-8') as file_obj:
        raw_data_list = file_obj.readlines()
    data_graph, max_user = load_graph_from_list(raw_data_list)
    print(data_graph, max_user)

    # 1. read the labels
    max_label = 0
    raw_data_list = []
    with open(os.path.join(full_path, label_file), encoding='utf-8') as file_obj:
        max_label = int(file_obj.readline())
        raw_data_list = file_obj.readlines()
    label_dict = load_label_from_list(raw_data_list, max_user=max_user)
    # print(label_dict[1+max_user])
    # 2. read the label name
    with open(os.path.join(full_path, label_name_file), encoding='utf-8') as file_obj:
        label_name_list = [line.rstrip() for line in file_obj.readlines()]
    # print(label_name_list)
    # 3. attach the label to nodes
    if len(label_name_file) > 0:
        for item_id, item_label_list in label_dict.items():
            item_label_name_list = [label_name_list[int(label)-1] for label in item_label_list]
            data_graph.nodes[item_id]["label"] = item_label_name_list
    # print(data_graph.nodes(data=True)[1+max_user])
    # print(label_dict, max_label)

    community_list = []
    # 4. extract communities
    # user_list_str = "1 4 9 15 39 53 272 396 519 1656 3088 3905"
    # item_list_str = "152 400 1590 1637 1643 1659 1674 1683 1709 1743 5538 11938 22487 26176 27064 29345 29500"
    # edge_list_str = "(1,152) (1,1637) (1,1643) (1,1659) (1,1683) (4,1637) (4,1659) (4,1683) (9,1637) (9,1643) (9,1659) (15,152) (15,1590) (15,1637) (15,1643) (15,1659) (15,1674) (15,1683) (15,1709) (15,1743) (15,22487) (15,29345) (15,29500) (39,152) (39,1709) (53,1637) (53,1659) (272,152) (272,400) (272,1590) (272,1674) (272,1709) (272,1743) (272,5538) (272,11938) (272,22487) (272,26176) (272,27064) (272,29345) (272,29500) (396,5538) (396,11938) (396,26176) (519,152) (519,1637) (519,1643) (519,1709) (1656,152) (1656,1709) (1656,22487) (3088,152) (3088,1709) (3905,400) (3905,5538) (3905,11938) (3905,26176) (3905,27064) (3905,29500)"
    # query_keywords_str = "1754 15587 511 9275 85476"

    # user_list_str = "4132 4283 4800 4936"
    # item_list_str = "438036 438037 438038 438039 438040 438041 438042 438043 438044 438045 550221 550222 550223 550224 550225 550226 550227 550228 550229 550230 550231 550232 550233 550234"
    # edge_list_str = "(4132,438036) (4132,438037) (4132,438038) (4132,438039) (4132,438040) (4132,438041) (4132,438042) (4132,438043) (4132,438044) (4132,438045) (4283,438036) (4283,438037) (4283,438038) (4283,438039) (4283,438040) (4283,438041) (4283,438042) (4283,438043) (4283,438044) (4283,438045) (4800,438036) (4800,438037) (4800,438038) (4800,438039) (4800,438040) (4800,438041) (4800,438042) (4800,438043) (4800,438044) (4800,438045) (4800,550221) (4800,550222) (4800,550223) (4800,550224) (4800,550225) (4800,550226) (4800,550227) (4800,550228) (4800,550229) (4800,550230) (4800,550231) (4800,550232) (4800,550233) (4800,550234) (4936,438036) (4936,438037) (4936,438038) (4936,438039) (4936,438040) (4936,438041) (4936,438042) (4936,438043) (4936,438044) (4936,438045) (4936,550221) (4936,550222) (4936,550223) (4936,550224) (4936,550225) (4936,550226) (4936,550227) (4936,550228) (4936,550229) (4936,550230) (4936,550231) (4936,550232) (4936,550233) (4936,550234)"
    # query_keywords_str = "242 7705 167250 776 2746"

    user_list_str = "4230 4658 4954"
    item_list_str = "443478 443479 443480 443481 443482 443483 443484 443485 443486"
    edge_list_str = "(4230,443478) (4230,443479) (4230,443480) (4230,443481) (4230,443482) (4230,443483) (4230,443484) (4230,443485) (4230,443486) (4658,443478) (4658,443479) (4658,443480) (4658,443481) (4658,443482) (4658,443483) (4658,443484) (4658,443485) (4658,443486) (4954,443480) (4954,443484) (4954,443485) (4954,443486)"
    query_keywords_str = "7705 9059 79997 185315 10721"

    user_list = [eval(user) for user in user_list_str.split(" ")]
    item_list = [eval(item)+max_user for item in item_list_str.split(" ")]
    edge_list = []
    for edge_str in edge_list_str.split(" "):
        [start, end] = edge_str[1:-1].split(",", 1)
        edge_list.append((int(start), int(end)+max_user))
    query_keyword_list = [label_name_list[eval(keyword_value)-1] for keyword_value in query_keywords_str.split(" ")]
    print(query_keyword_list)

    # k_r_sigma_bitruss = data_graph.subgraph(nodes=user_list+item_list)
    # for user_id in user_list:
    #     data_graph.nodes[user_id]["is_community"] = 1

    # for item_id in item_list:
    #     data_graph.nodes[item_id]["is_community"] = 1

    # print(k_r_sigma_bitruss)
    local_graph_user_list = []
    radius = 1
    for user_id in tqdm(user_list):
        items_in_hop = set(item_list)
        users_in_hop = set(user_list)
        to_traverse_item = []
        to_traverse_user = [user_id]
        for r_dev in range(radius):
            for user_node in to_traverse_user:
                for item_neighbor in data_graph.neighbors(user_node):
                    if item_neighbor not in items_in_hop:
                        to_traverse_item.append(item_neighbor)
            to_traverse_user = []
            for item_node in to_traverse_item:
                for user_neighbor in data_graph.neighbors(item_node):
                    if user_neighbor not in users_in_hop:
                        items_in_hop.add(item_node)
                        users_in_hop.add(user_neighbor)
                        to_traverse_user.append(user_neighbor)
            to_traverse_item = []
        for idx, user_node in tqdm(enumerate(to_traverse_user)):
            # if idx < 10:
            users_in_hop.add(user_node)
    # local_graph = data_graph.subgraph(nodes=local_graph_user_list)
    # print(local_graph)
    community_list.append([[user_list, item_list, edge_list], [list(users_in_hop), list(items_in_hop)]])

    # biclique result
    community_super_graph = data_graph.subgraph(nodes=list(users_in_hop)+list(items_in_hop))
    biclique_item_list = set(data_graph.neighbors(user_list[0]))
    for user_id in user_list:
        # print(biclique)
        biclique_item_list = biclique_item_list & set(data_graph.neighbors(user_id))
        # print(len(biclique_item_list))
    biclique_item_list = list(biclique_item_list)
    # print(biclique_item_list)
    biclique_edge_list = list(product(user_list, biclique_item_list))
    community_list.append([[user_list, biclique_item_list, biclique_edge_list], [list(users_in_hop), list(items_in_hop)]])

    '''
    for index, (k_r_sigma_bitruss, local_graph) in enumerate(community_list):
        plt.cla()
        plt.axis('off')
        whole_graph = data_graph.subgraph(local_graph[0] + local_graph[1]).copy()
        seed_pos = nx.bipartite_layout(
            G=nx.subgraph(G=whole_graph, nbunch=k_r_sigma_bitruss[0] + k_r_sigma_bitruss[1]),
            nodes=k_r_sigma_bitruss[0],
            center=[k_r_sigma_bitruss[0][0], k_r_sigma_bitruss[1][0]]
        )
        pos = nx.bipartite_layout(
            G=whole_graph,
            nodes=nx.bipartite.sets(whole_graph)[0]
        )
        # print([node[1] for node in whole_graph.nodes(data=True)])
        # for node in whole_graph.nodes(data=True):
        #      if node[1]["is_k_r_sigma_bitruss"]:
        color_list = [weight for start, end, weight in whole_graph.edges(data="weight")]
        # print(color_list)
        nx.draw_networkx(G=whole_graph, pos=pos, with_labels=False, node_size=20, width=0.1,
                         node_color="#000000",
                         edge_vmin=0.1, edge_vmax=0.5, linewidths=0.1,
                         edge_color="#000000")
                        # edge_cmap=plt.cm.Reds, edge_color=color_list)
        nx.draw_networkx_nodes(G=whole_graph, pos=pos, node_shape="s", nodelist=local_graph[1], node_color="#000000", node_size=20)
        community_subgraph = nx.subgraph(whole_graph, nbunch=k_r_sigma_bitruss[0] + k_r_sigma_bitruss[1])
        # nx.draw_networkx_nodes(G=whole_graph, pos=pos, node_shape="o", nodelist=[150867, 165437, 444519, 513693], node_color='r', node_size=300)
        nx.draw_networkx_nodes(G=whole_graph, pos=pos, node_shape="o", nodelist=k_r_sigma_bitruss[0], node_color='r', node_size=50)
        nx.draw_networkx_nodes(G=whole_graph, pos=pos, node_shape="s", nodelist=k_r_sigma_bitruss[1], node_color='r', node_size=50)
        nx.draw_networkx_edges(G=whole_graph, pos=pos, edgelist=k_r_sigma_bitruss[2], edge_color='r', width=0.2)
        plt.savefig("./{0}_{1}_{2:.2f}.png".format(
            len(k_r_sigma_bitruss[0] + k_r_sigma_bitruss[1]),
            len(local_graph[0] + local_graph[1]),
            index
        ), dpi=1000, bbox_inches='tight')
    '''
    plt.cla()
    plt.figure(figsize=(50, 28))
    plt.axis('off')
    whole_graph = data_graph.subgraph(list(users_in_hop) + list(items_in_hop)).copy()
    user_pos = nx.bipartite_layout(
        G=nx.subgraph(G=whole_graph, nbunch=user_list + item_list),
        nodes=item_list, aspect_ratio=4, scale=10, align="horizontal"
    )
    # print(user_pos.items())
    # moved_user_pos = dict()
    # for node, (pos_x, pos_y) in user_pos.items():
    #     if node in item_list:
    #         moved_user_pos[node] = np.array([-0.7, pos_y])
    #     else:
    #         moved_user_pos[node] = np.array([pos_x, pos_y])
    # user_pos = moved_user_pos
    print(user_pos.items())
    user_pos = dict([
        (449272, np.array([-1.66666667, -3.33333333])),
        (449273, np.array([-1.25      , -3.33333333])),
        (449277, np.array([-0.83333333, -3.33333333])),
        (449275, np.array([-0.41666667, -3.33333333])),
        (449276, np.array([ 0.        , -3.33333333])),
        (449274, np.array([ 0.41666667, -3.33333333])),
        (449278, np.array([ 0.83333333, -3.33333333])),
        (449279, np.array([ 1.25      , -3.33333333])),
        (449280, np.array([ 1.66666667, -3.33333333])),
        (4230, np.array([-1.66666667, 10.        ])),
        (4658, np.array([ 0., 10.])),
        (4954, np.array([ 1.66666667, 10.        ]))
    ])

    # item_pos = nx.bipartite_layout(
    #     G=whole_graph,
    #     nodes=nx.bipartite.sets(whole_graph)[1]
    # )
    # print([node[1] for node in whole_graph.nodes(data=True)])
    # for node in whole_graph.nodes(data=True):
    #      if node[1]["is_k_r_sigma_bitruss"]:
    # color_list = [weight for start, end, weight in whole_graph.edges(data="weight")]
    # print(color_list)
    # nx.draw_networkx(G=whole_graph, pos=user_pos, with_labels=False, node_size=200, width=0.1,
    #                  node_color="#000000", linewidths=0.1, edge_color="#000000")  # edge_cmap=plt.cm.Reds, edge_color=color_list)
    # nx.draw_networkx_nodes(G=whole_graph, pos=pos, node_shape="s", nodelist=list(items_in_hop), node_color="#000000", node_size=20)
    # nx.draw_networkx_nodes(G=whole_graph, pos=pos, node_shape="o", nodelist=[150867, 165437, 444519, 513693], node_color='r', node_size=300)

    # k_r_sigma_bitruss
    nx.draw_networkx_nodes(
        G=whole_graph, pos=user_pos, node_shape="*",
        nodelist=user_list, node_color='r', node_size=30000, label="CD-SBN Community User"
    )
    # 30000
    nx.draw_networkx_nodes(
        G=whole_graph, pos=user_pos, node_shape="D",
        nodelist=list(set(item_list) - set(biclique_item_list)),
        node_color='r', node_size=8000, label="CD-SBN Community Item"
    )
    # 10000
    nx.draw_networkx_edges(
        G=whole_graph, pos=user_pos, edgelist=list(set(edge_list)-set(biclique_edge_list)),
        edge_color='r', width=3.0
    )

    # biclique
    # nx.draw_networkx_nodes(
    #     G=whole_graph, pos=user_pos, node_shape="o",
    #     nodelist=user_list, node_color="#396ED6", node_size=12000
    # )
    nx.draw_networkx_nodes(
        G=whole_graph, pos=user_pos, node_shape="s",
        nodelist=biclique_item_list, node_color="#396ED6", node_size=12000, label="CD-SBN and Biclique Common Item"
    )
    nx.draw_networkx_edges(
        G=whole_graph, pos=user_pos, edgelist=biclique_edge_list,
        edge_color="#396ED6", width=3.0, style="dashed"
    )

    # Item Keyword labels
    # whole_graph_item_label_dict = dict()
    # for node in nx.bipartite.sets(whole_graph)[0]:
    #     # print(node)
    #     whole_graph_item_label_dict[node] = whole_graph.nodes[node]["label"]
    # for item_id, label_list in whole_graph_item_label_dict.items():
    #     whole_graph_item_label_dict[item_id] = "\"{0}\"".format([keyword for keyword in label_list if keyword in query_keyword_list][0])
    # # print(whole_graph_item_label_dict)
    # label_pos = {node: np.array([pos_x, pos_y-1.2]) for node, (pos_x, pos_y) in user_pos.items()}
    # nx.draw_networkx_labels(
    #     G=whole_graph, pos=label_pos, labels=whole_graph_item_label_dict,
    #     font_color="#000000", font_size=36, horizontalalignment="center",
    #     font_family="Times New Roman"
    # )

    # ID labels
    user_id_label_pos = dict()
    for node, (pos_x, pos_y) in user_pos.items():
        if node == 4954:
            user_id_label_pos[4954] = np.array([pos_x-0.3, pos_y])
        else:
            user_id_label_pos[node] = np.array([pos_x+0.3, pos_y])

    item_id_label_pos = {node: np.array([pos_x, pos_y-1.0]) for node, (pos_x, pos_y) in user_pos.items()}
    # id_label_pos = user_pos
    nx.draw_networkx_labels(
        G=whole_graph, pos=user_id_label_pos, labels={node: "u_{}".format(node) for node in (user_list)},
        font_color="#000000", font_size=72, horizontalalignment="center",
        font_weight="normal", font_family="Times New Roman"
    )
    nx.draw_networkx_labels(
        G=whole_graph, pos=item_id_label_pos, labels={node: "v_{}".format(node) for node in (item_list)},
        font_color="#000000", font_size=70, horizontalalignment="center",
        font_weight="bold", font_family="Times New Roman"
    )

    # Edge weight labels
    community_edge_label_dict = dict()
    biclique_edge_label_list = {user_id: dict() for user_id in user_list}
    for u, v, weight in whole_graph.edges.data("weight"):
        if u < v:
            if v in biclique_item_list:
                biclique_edge_label_list[u][(u, v)] = weight
            else:
                community_edge_label_dict[(u, v)] = weight
        else:
            if u in biclique_item_list:
                biclique_edge_label_list[v][(v, u)] = weight
            else:
                community_edge_label_dict[(v, u)] = weight
    label_bbox = dict(boxstyle="round", ec=(1.0, 1.0, 1.0), fc=(1.0, 1.0, 1.0), alpha=0.7)
    nx.draw_networkx_edge_labels(
        G=whole_graph, pos=user_pos, edge_labels=community_edge_label_dict,
        font_color="#000000", font_size=54, label_pos=0.9, bbox=label_bbox, rotate=False,
        font_family="Times New Roman"
    )
    for idx, (user_id, biclique_edge_label_dict) in enumerate(biclique_edge_label_list.items()):
        nx.draw_networkx_edge_labels(
            G=whole_graph, pos=user_pos, edge_labels=biclique_edge_label_dict,
            font_color="#000000", font_size=54, label_pos=0.80 - idx * 0.1, bbox=label_bbox, rotate=False,
            font_family="Times New Roman"
        )

    plt.axis('off')
    # plt.legend(prop={"family": "Times New Roman"})
    plt.savefig("./{0}_{1}_{2:.2f}.png".format(
        len(user_list + item_list),
        len(list(users_in_hop) + list(items_in_hop)),
        2
    ), dpi=120, bbox_inches="tight")
