import os
import numpy as np
import networkx as nx
from tqdm import tqdm


def generate_power_law_bipartite_graph(user_num: int, item_num: int, alpha: int):
    # Generate power-law degree sequences
    top_degrees = np.random.zipf(alpha, user_num)
    bottom_degrees = np.random.zipf(alpha, item_num)
    # Adjust degree sequences to ensure their sums match
    total_top_degrees = np.sum(top_degrees)
    total_bottom_degrees = np.sum(bottom_degrees)
    if total_top_degrees != total_bottom_degrees:
        # Scale the degree sequences to match the total degrees
        scaling_factor = (total_bottom_degrees*1.0) / (total_top_degrees*1.0)
        top_degrees = np.round(top_degrees * scaling_factor).astype(int)
        total_top_degrees = np.sum(top_degrees)
        total_bottom_degrees = np.sum(bottom_degrees)
    if total_top_degrees > total_bottom_degrees:
        increment_value = total_top_degrees - total_bottom_degrees
        for _ in range(increment_value):
            idx = np.random.randint(0, len(bottom_degrees))
            bottom_degrees[idx] += 1
    elif total_top_degrees < total_bottom_degrees:
        increment_value = total_bottom_degrees - total_top_degrees
        for _ in range(increment_value):
            idx = np.random.randint(0, len(top_degrees))
            top_degrees[idx] += 1

    # Create bipartite graph using configuration model
    B = nx.bipartite.configuration_model(top_degrees, bottom_degrees)
    B = nx.Graph(B)  # Convert to simple graph
    B.remove_edges_from(nx.selfloop_edges(B))  # Remove self-loops
    return B


# For the synthetic graphs, we generate a power-law graph (PL) in which edges are randomly added such that
# the degree distribution follows a power-law distribution and a uniform-degree graph (UD) in which
# all edges are added with the same probability.
def generate_synthetic_dataset_powerlaw(
    dir_name: str,
    file_name: str,
    user_num: int,
    item_num: int,
    alpha: float,
    p: float
):
    print("Process [{}]".format(dir_name))
    # 1. generate bipartite grpah by nx
    data_graph = generate_power_law_bipartite_graph(user_num=user_num, item_num=item_num, alpha=alpha)
    # 2. extract the edge list
    edge_list = []
    # print(data_graph.edges)
    for edge in tqdm(data_graph.edges):
        edge_list.append((edge[0], edge[1]-user_num))
    # edge_set = set(edge_list)
    for edge in tqdm(edge_list):
        [flag] = np.random.choice([0, 1], size=1, p=[1-p, p])
        if flag > 0:
            edge_list.append((edge[0], edge[1]))
    # 3. save the edge file
    graph_file_path = os.path.join(dir_name, file_name)
    full_file_path = (os.path.join(os.getcwd(), "dataset", graph_file_path))
    with open(full_file_path, "w") as wf:
        wf.write("% {} {} {}\n".format(user_num, item_num, len(edge_list)))
        for edge in tqdm(edge_list):
            wf.write("{} {}\n".format(edge[0], edge[1]))


# For the synthetic graphs, we generate a power-law graph (PL) in which edges are randomly added such that
# the degree distribution follows a power-law distribution and a uniform-degree graph (UD) in which
# all edges are added with the same probability.
def generate_synthetic_dataset_uniform(
    dir_name: str,
    file_name: str,
    user_num: int,
    item_num: int,
    add_edge_probability: float,
    p: float
):
    print("Process [{}]".format(dir_name))
    # 1. generate bipartite grpah by nx
    data_graph = nx.bipartite.random_graph(user_num, item_num, add_edge_probability)
    # 2. extract the edge list
    edge_list = []
    # print(data_graph.edges)
    for edge in tqdm(data_graph.edges):
        edge_list.append((edge[0], edge[1]-user_num))
    # edge_set = set(edge_list)
    for edge in tqdm(edge_list):
        [flag] = np.random.choice([0, 1], size=1, p=[1-p, p])
        if flag > 0:
            edge_list.append((edge[0], edge[1]))
    # 3. save the edge file
    graph_file_path = os.path.join(dir_name, file_name)
    full_file_path = (os.path.join(os.getcwd(), "dataset", graph_file_path))
    with open(full_file_path, "w") as wf:
        wf.write("% {} {} {}\n".format(user_num, item_num, len(edge_list)))
        for edge in tqdm(edge_list):
            wf.write("{} {}\n".format(edge[0], edge[1]))


if __name__ == "__main__":
    generate_synthetic_dataset_powerlaw(
        "PL",
        "out.pl-50k",
        50000,
        50000,
        1.5,
        0.1
    )
    generate_synthetic_dataset_uniform(
        "UD",
        "out.ud-50k",
        50000,
        50000,
        0.0005,
        0.1
    )
