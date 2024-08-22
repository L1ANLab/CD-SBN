import os
import numpy as np
import networkx as nx
from tqdm import tqdm
from scipy.stats import uniform, powerlaw, beta
from matplotlib import pyplot as plt
from distfit import distfit

# For the synthetic graphs, we generate three graphs in which edges are randomly added
# a power-law graph (PL) such that the degree distribution follows a power-law distribution
# a beta graph such that the degree distribution follows a beta distribution
# ,and a uniform-degree graph (UD) such that all edges are added with the same probability.

ADD_EDGE_PROBABILIRY = 0.1


def generate_graph_by_distr(dir_name: str, user_num: int, item_num: int, distr: str):
    # 1. Generate degree sequences
    top_degree_list = []
    bottom_degree_list = []

    if distr == "uniform":
        print("Do Uniform")
        loc_param = 10
        scale_param = 10
        top_degree_list = []
        bottom_degree_list = []
        while len(top_degree_list) < user_num:
            add_size = user_num - len(top_degree_list)
            top_degree_list.extend(uniform.rvs(size=add_size, loc=loc_param, scale=scale_param))
            top_degree_list = [num for num in top_degree_list if num >= loc_param and num < loc_param+scale_param]
        while len(bottom_degree_list) < item_num:
            add_size = item_num - len(bottom_degree_list)
            bottom_degree_list.extend(uniform.rvs(size=add_size, loc=loc_param, scale=scale_param))
            bottom_degree_list = [num for num in bottom_degree_list if num >= loc_param and num < loc_param+scale_param]
    elif distr == "powerlaw":
        print("Do Power-Law")
        # AM25k  0.9999999999999999  ,        26111.000000000004    ,"(0.9200109488572684,)"
        # AR2M   0.9999999999999999  ,2146056.0000000005      ,"(0.4080323204109871,)"
        # BS5k   0.9999999999999999  ,        5793.000000000001   ,"(0.7350243508148506,)"
        # CM15k  0.9999999999999999  ,17614.000000000004    ,"(0.4406414158701184,)"
        # CU25k  0.9999999999999999  ,        22714.000000000004    ,"(0.8208127548255336,)"
        # DV150k 0.9999999999999999  , 139408.00000000003     ,"(0.3834060605893954,)"
        # ML5k   0.9999999999999999  ,        4008.0000000000005    ,"(0.9511868424803924,)"
        # SX10k  0.9999999999999999  ,       10105.000000000002    ,"(0.7693565782903532,)"
        # TA15k  0.9999999999999999  ,        145315.00000000003     ,"(0.8890303389702157,)"
        # UF1k   0.9999999999999999   ,        898.0000000000001    ,"(0.6569864709983231,)"
        # VU15k  0.9999999999999999  , 82034.00000000001     ,"(0.20889796629344404,)"
        # WU150k 0.9999999999999999 ,       175213.00000000003     ,"(0.6129378066141103,)"
        arg_param = 0.04
        loc_param = 1
        top_scale_param = user_num/200
        bottom_scale_param = item_num/200
        top_degree_list = []
        bottom_degree_list = []
        while len(top_degree_list) < user_num:
            add_size = user_num - len(top_degree_list)
            top_degree_list.extend(powerlaw.rvs(a=arg_param, size=add_size, loc=loc_param, scale=top_scale_param))
            top_degree_list = [num for num in top_degree_list if num >= 0 and num < top_scale_param]
        while len(bottom_degree_list) < item_num:
            add_size = item_num - len(bottom_degree_list)
            bottom_degree_list.extend(powerlaw.rvs(a=arg_param, size=add_size, loc=loc_param, scale=bottom_scale_param))
            bottom_degree_list = [num for num in bottom_degree_list if num >= 0 and num < bottom_scale_param]
    elif distr == "beta":
        print("Do Beta")
        # AM25k  0.9999999999999947  ,        26111.240065647613    ,"(0.8505395749237528, 0.9890139435771843)"
        # AR2M   0.9999999999999999  ,2146102.2926170863      ,"(0.44334960225152875, 0.7698335258815061)"
        # BS5k   0.9999999999988536  ,        5843.311390698574   ,"(0.8600317088730833, 0.949727993577901)"
        # CM15k  0.9999999999999999  ,17864.006310168566    ,"(0.5280776945808431, 1.5228656187116267)"
        # CU25k  0.9999999999999999  ,        22714.163579620326    ,"(0.9357576429704562, 1.2318726974666112)"
        # DV150k 0.9999999999999999  ,1425718.4494510037      ,"(0.6507850157951769, 34.90112793218721)"
        # ML5k   0.9999999999999997  ,        4008.05443641274      ,"(0.9812087807923604, 1.0490710950397468)"
        # SX10k  0.9999999999999949  ,       10110.407743319516    ,"(0.9608971612276223, 1.4930368841510693)"
        # TA15k  -6.806916741512913  ,        145322.80691674154     ,"(0.8117947283352374, 0.8644583796076639)"
        # UF1k   0.9999999999999999   ,        902.4417624619954    ,"(0.9258096236521509, 1.7208744945675574)"
        # VU15k  0.9999999999999999  ,678264.8818477776      ,"(0.6209163154972581, 210.52780274892902)"
        # WU150k 0.9999999999999999 ,       175223.061925438       ,"(0.7872391787005166, 1.44342138235131)"
        arg_a_param = 0.04
        arg_b_param = 1
        loc_param = 1
        top_scale_param = user_num/200
        bottom_scale_param = item_num/200
        top_degree_list = []
        bottom_degree_list = []
        while len(top_degree_list) < user_num:
            add_size = user_num - len(top_degree_list)
            top_degree_list.extend(beta.rvs(a=arg_a_param, b=arg_b_param, size=add_size, loc=loc_param, scale=top_scale_param))
            top_degree_list = [num for num in top_degree_list if num >= 0 and num < top_scale_param]
        while len(bottom_degree_list) < item_num:
            add_size = item_num - len(bottom_degree_list)
            bottom_degree_list.extend(beta.rvs(a=arg_a_param, b=arg_b_param, size=add_size, loc=loc_param, scale=bottom_scale_param))
            bottom_degree_list = [num for num in bottom_degree_list if num >= 0 and num < bottom_scale_param]
    else:
        raise Exception("Type Error: wrong distribution <{}>".format(distr))

    top_degree_list = np.clip(top_degree_list, 0, user_num/200).astype(int)
    bottom_degree_list = np.clip(bottom_degree_list, 0, item_num/200).astype(int)
    print(top_degree_list[:100])
    print(max(top_degree_list))
    # 2. Adjust degree sequences to ensure their sums match
    total_top_degree_sum = np.sum(top_degree_list)
    total_bottom_degree_sum = np.sum(bottom_degree_list)
    print(total_top_degree_sum, total_bottom_degree_sum)
    if total_top_degree_sum != total_bottom_degree_sum:
        # Scale the degree sequences to match the total degrees
        scaling_factor = (total_bottom_degree_sum*1.0) / (total_top_degree_sum*1.0)
        top_degree_list = np.round(top_degree_list * scaling_factor).astype(int)
    total_top_degree_sum = np.sum(top_degree_list)
    total_bottom_degree_sum = np.sum(bottom_degree_list)

    distr_list = ["beta", "uniform", "powerlaw"]
    dfit = distfit(todf=True, distr=distr_list)
    dfit.fit_transform(top_degree_list)
    dfit.plot()
    fig_path = os.path.join(os.getcwd(), "dataset", "synthetic", dir_name, "./{}_user_degree_vis".format(dir_name))
    plt.savefig(fig_path)

    dfit.plot_summary()
    fig_path = os.path.join(os.getcwd(), "dataset", "synthetic", dir_name, "./{}_user_degree_vis_summary".format(dir_name))
    plt.savefig(fig_path)

    output_path = os.path.join(os.getcwd(), "dataset", "synthetic", dir_name, "user_degree_distribution_fit.csv")
    dfit.summary.to_csv(output_path, sep=',', index=True, header=True)

    # 3. clip each element until the equal degree sum
    print(total_top_degree_sum/user_num, total_bottom_degree_sum/item_num)
    if total_top_degree_sum > total_bottom_degree_sum:
        increment_value = total_top_degree_sum - total_bottom_degree_sum
        for _ in range(increment_value):
            idx = np.random.randint(0, len(bottom_degree_list))
            bottom_degree_list[idx] += 1
    elif total_top_degree_sum < total_bottom_degree_sum:
        increment_value = total_bottom_degree_sum - total_top_degree_sum
        for _ in range(increment_value):
            idx = np.random.randint(0, len(top_degree_list))
            top_degree_list[idx] += 1

    print(total_top_degree_sum/user_num, total_bottom_degree_sum/item_num)
    # Create bipartite graph using configuration model
    B = nx.bipartite.configuration_model(top_degree_list, bottom_degree_list)
    B = nx.Graph(B)  # Convert to simple graph
    B.remove_edges_from(nx.selfloop_edges(B))  # Remove self-loops
    return B


def generate_synthetic_dataset(
    dir_name: str,
    file_name: str,
    user_num: int,
    item_num: int,
    distr: str,
):
    print("Process [{}]".format(file_name))
    # 1. generate bipartite graph for different type
    data_graph = generate_graph_by_distr(dir_name, user_num, item_num, distr)
    # 2. extract the edge list
    edge_list = []
    # print(data_graph.edges)
    for edge in tqdm(data_graph.edges):
        edge_list.append((edge[0], edge[1]-user_num))
    # edge_set = set(edge_list)
    for idx, edge in enumerate(tqdm(edge_list)):
        [flag] = np.random.choice([0, 1], size=1, p=[1-ADD_EDGE_PROBABILIRY, ADD_EDGE_PROBABILIRY])
        if flag > 0:
            edge_list.insert(idx, (edge[0], edge[1]))
    # 3. save the edge file
    graph_file_path = os.path.join(dir_name, file_name)
    full_file_path = (os.path.join(os.getcwd(), "dataset", "synthetic", graph_file_path))
    with open(full_file_path, "w") as wf:
        wf.write("% {} {} {}\n".format(user_num, item_num, len(edge_list)))
        for edge in tqdm(edge_list):
            wf.write("{} {}\n".format(edge[0], edge[1]))


if __name__ == "__main__":
    generate_synthetic_dataset(
        "dP",
        "out.pl-25k",
        25000,
        25000,
        "powerlaw"
    )
    generate_synthetic_dataset(
        "dB",
        "out.bd-25k",
        25000,
        25000,
        "beta"
    )