import os

import numpy as np
from scipy.stats import lognorm, pareto
from tqdm import tqdm
from matplotlib import pyplot as plt
from distfit import distfit


INITIAL_GRAPH_RATIO = 0.9999


class EdgeRawData:
    def __init__(self, user, item, weight, tstamp):
        self.user = user
        self.item = item
        self.weight = weight
        self.tstamp = tstamp

    def __lt__(self, other):
        if self.tstamp < other.tstamp:
            return True
        elif self.tstamp > other.tstamp:
            return False
        else:
            if self.user < other.user:
                return True
            elif self.user > other.user:
                return False
            else:
                if self.item < other.item:
                    return True
                else:
                    return False

    def __str__(self) -> str:
        return ("{} {} {}").format(self.user, self.item, self.tstamp)


def generate_dataset(
    dataset_type: str,
    dir_name: str,
    file_name: str,
    distribution: str,
    all_keyword_num: int,
    keywords_per_vertex_num: int
):
    print("Process [{}]".format(dir_name))
    # 0. Param settings
    file_path = os.path.join(dataset_type, dir_name, file_name)

    # 1. File read
    full_file_path = (os.path.join(os.getcwd(), "dataset", file_path))
    raw_data_list = []
    with open(full_file_path, "r") as f:
        raw_data_list = f.readlines()

    # 2. Raw data process
    max_item = 0
    min_tstamp = float("inf")
    max_tstamp = 0

    data_list = []
    counter = 0
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
        for _ in range(int(weight)):
            now_data = EdgeRawData(int(user), int(item), 1, int(tstamp))
            data_list.append(now_data)
            # now_data = EdgeRawData(int(user), int(item), int(weight), int(tstamp))
            # data_list.append(now_data)
            # print(now_data.item)
            if now_data.item > max_item:
                max_item = now_data.item
            if now_data.item > max_tstamp:
                max_tstamp = now_data.tstamp
            if now_data.tstamp < min_tstamp:
                min_tstamp = now_data.tstamp

            # if now_data.weight > 1:
            #     print(now_data)
        counter += 1
    data_list = sorted(data_list)

    # 3. divide inital graph and update stream
    initial_graph_size = int(len(data_list) * INITIAL_GRAPH_RATIO)
    initial_graph = data_list[:initial_graph_size]
    update_stream = data_list[initial_graph_size:]

    # 4. generate distribution sample
    item_label_list = []
    keywords_set = []
    if distribution == "uniform":
        keywords_set = range(0, all_keyword_num)
    elif distribution == "pareto":
        # Pareto
        arg_param = 1.5
        loc_param = 1
        scale_param = all_keyword_num
        if all_keyword_num == 15000:  # -19231.79008351339  ,19232.790083513388     ,"(5.69997408561057,)"
            arg_param = 5.70
            loc_param = -19231.79
            scale_param = 19232.79
        elif all_keyword_num == 80000:  # -388.84576533729086 ,   389.8457653372908   ,"(0.6901126943354385,)"
            arg_param = 0.69
            loc_param = -389
            scale_param = 390
        elif all_keyword_num == 150000:  # -4124.0358721624125 ,     4125.035872162412   ,"(0.8103013210457232,)"
            arg_param = 0.81
            loc_param = -4124
            scale_param = 4125
        elif all_keyword_num == 200000:  # -3401.5253467891953  ,  3402.525346789195    ,"(0.5464479455054979,)"
            arg_param = 0.55
            loc_param = -3401
            scale_param = 3402
        size = max_item + 1
        while len(keywords_set) < size * keywords_per_vertex_num:
            add_size = size * keywords_per_vertex_num - len(keywords_set)
            keywords_set.extend(pareto.rvs(b=arg_param, size=add_size, loc=loc_param, scale=scale_param))
            keywords_set = [num for num in keywords_set if num >= 0 and num < all_keyword_num]
    elif distribution == "lognorm":
        # Log-Normal
        arg_param = 1.5
        loc_param = 0
        scale_param = all_keyword_num
        if all_keyword_num == 500:
            arg_param = 2.38
            loc_param = 1
            scale_param = 250
        elif all_keyword_num == 15000:  # -44.58722103972769, 1839.072875086526     ,"(1.5010218174412135,)"
            arg_param = 1.5
            loc_param = -44.58
            scale_param = 1839
        elif all_keyword_num == 80000:  # -0.5932967914583205 ,   808.7013050884292   ,"(1.9873671880000185,)"
            arg_param = 1.987
            loc_param = -0.59
            scale_param = 800
        elif all_keyword_num == 150000:  # -4.551298755677167  ,     5024.638869027135   ,"(2.1871830415933924,)"
            arg_param = 2.18
            loc_param = -4.55
            scale_param = 5024
        elif all_keyword_num == 200000:  # -0.11391367692277611 ,  9656.598374650848    ,"(2.3804364903581736,)"
            arg_param = 2.38
            loc_param = -0.11
            scale_param = 9656.6
        # lognorm_dist = lognorm(s=arg_param)
        size = max_item + 1
        # keywords_set = lognorm.rvs(s=arg_param, size=size * keywords_per_vertex_num, loc=loc_param, scale=scale_param)
        print("Do Log-Normal")
        while len(keywords_set) < size * keywords_per_vertex_num:
            add_size = size * keywords_per_vertex_num - len(keywords_set)
            keywords_set.extend(lognorm.rvs(s=arg_param, size=add_size, loc=loc_param, scale=scale_param))
            keywords_set = [num for num in keywords_set if num >= 0 and num < all_keyword_num]
    keywords_set = np.clip(keywords_set, 0, all_keyword_num-1).astype(int)

    # 5. generate keywords following distribution
    all_keyword_list = []
    label_counter = [0 for _ in range(all_keyword_num)]
    for item_id in range(max_item+1):
        keyword_num = np.random.randint(max(keywords_per_vertex_num-1, 1), keywords_per_vertex_num+2)  # the num is key_per ± 1

        keywords = np.random.choice(keywords_set, keyword_num)
        while len(set(keywords)) != len(keywords):
            keywords = np.random.choice(keywords_set, keyword_num)
        all_keyword_list.extend(keywords)
        for keyword in keywords:
            label_counter[keyword] += 1
        item_label_list.append(keywords)

    print(label_counter)
    all_keyword_list = np.array(all_keyword_list)
    distr_list = ['pareto', "lognorm", "uniform"]
    dfit = distfit(todf=True, distr=distr_list)
    dfit.fit_transform(all_keyword_list)
    dfit.plot()
    fig_path = os.path.join(os.getcwd(), "dataset", dataset_type, dir_name, "./{}_keyword_vis".format(dir_name))
    plt.savefig(fig_path)

    dfit.plot_summary()
    fig_path = os.path.join(os.getcwd(), "dataset", dataset_type, dir_name, "./{}_keyword_vis_summary".format(dir_name))
    plt.savefig(fig_path)

    output_path = os.path.join(os.getcwd(), "dataset", dataset_type, dir_name, "keyword_distribution_fit.csv")
    dfit.summary.to_csv(output_path, sep=',', index=True, header=True)

    # 6. write dataset into files
    initial_graph_file = "initial_graph.txt"
    item_label_list_file = "label_list.txt"
    update_stream_file = "update_stream.txt"
    keyword_file = "keywords_list.txt"
    print("Save Graph Data")
    with open(os.path.join(os.getcwd(), "dataset", dataset_type, dir_name, initial_graph_file), "w") as wf:
        for data in tqdm(initial_graph):
            wf.write("{} {} {}\n".format(data.user, data.item, data.tstamp))

    print("Save Label Data")
    with open(os.path.join(os.getcwd(), "dataset", dataset_type, dir_name, item_label_list_file), "w") as wf:
        wf.write("{}\n".format(all_keyword_num))
        for item_id, labels in tqdm(enumerate(item_label_list)):
            wf.write("{} {}\n".format(item_id, ",".join([str(label) for label in labels])))

    print("Save Update Stream Data")
    with open(os.path.join(os.getcwd(), "dataset", dataset_type, dir_name, update_stream_file), "w") as wf:
        wf.write("% min:{} max:{}\n".format(min_tstamp, max_tstamp))
        for data in tqdm(update_stream):
            wf.write("{} {} {}\n".format(data.user, data.item, data.tstamp))

    print("Save Keyword Data")
    with open(os.path.join(os.getcwd(), "dataset", dataset_type, dir_name, keyword_file), "w") as wf:
        wf.write("{}\n".format(all_keyword_num))
        for keyword in tqdm(all_keyword_list):
            wf.write("{}\n".format(keyword))


def generate_dataset_with_keywords(dataset_type: str, dir_name: str, graph_file_name: str, keyword_file_name: str):
    print("Process [{}]".format(dir_name))
    # 0. Param settings
    graph_file_path = os.path.join(dataset_type, dir_name, graph_file_name)
    keyword_file_path = os.path.join(dataset_type, dir_name, keyword_file_name)

    # 1. Graph file read
    full_file_path = (os.path.join(os.getcwd(), "dataset", graph_file_path))
    raw_data_list = []
    with open(full_file_path, "r") as f:
        raw_data_list = f.readlines()

    # 2. Raw data process
    max_item = 0
    min_tstamp = float("inf")
    max_tstamp = 0

    print("Process Graph Data")
    data_list = []
    counter = 0
    for line in tqdm(raw_data_list):
        if line.startswith("%"):
            continue
        # [user, item, weight, tstamp] = line.strip().split(" ")

        line_list = line.strip().split(" ")
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
        for _ in range(int(weight)):
            now_data = EdgeRawData(int(user), int(item), 1, int(tstamp))
            data_list.append(now_data)
            # now_data = EdgeRawData(int(user), int(item), int(weight), int(tstamp))
            # data_list.append(now_data)
            if now_data.item > max_item:
                max_item = now_data.item
            if now_data.item > max_tstamp:
                max_tstamp = now_data.tstamp
            if now_data.tstamp < min_tstamp:
                min_tstamp = now_data.tstamp
            if now_data.weight > 1:
                print(now_data)
        counter += 1
    data_list = sorted(data_list)

    # 3. divide inital graph and update stream
    initial_graph_size = int(len(data_list) * INITIAL_GRAPH_RATIO)
    initial_graph = data_list[:initial_graph_size]
    update_stream = data_list[initial_graph_size:]

    # 4. read the keyword file
    full_keyword_file_path = (os.path.join(os.getcwd(), "dataset", keyword_file_path))
    raw_keyword_data_list = []
    with open(full_keyword_file_path, "r") as f:
        raw_keyword_data_list = f.readlines()

    # 5. keyword data process
    max_keyword = 0
    keyword_list = dict()
    all_keyword_list = []
    counter = 0
    print("Process Keyword Data")
    for line in tqdm(raw_keyword_data_list):
        if line.startswith("%"):
            continue
        line_list = line.strip().split(" ")
        keyword = 0
        item = 0
        weight = 1
        tstamp = counter
        if len(line_list) == 2:
            [keyword, item] = line_list
        elif len(line_list) == 4:
            [keyword, item, weight, tstamp] = line_list
        keyword = int(float(keyword))
        weight = int(float(weight))
        tstamp = int(float(tstamp))
        if item not in keyword_list:
            keyword_list[item] = [keyword]
        elif keyword not in keyword_list[item]:
            keyword_list[item].append(keyword)
        all_keyword_list.append(keyword)

        if keyword > max_keyword:
            max_keyword = keyword
        counter += 1
    keyword_list = sorted(keyword_list.items(), key=lambda d: int(d[0]), reverse=False)

    # 6. write dataset into files
    initial_graph_file = "initial_graph.txt"
    item_label_list_file = "label_list.txt"
    update_stream_file = "update_stream.txt"
    keyword_file = "keywords_list.txt"
    print("Save Graph Data")
    with open(os.path.join(os.getcwd(), "dataset", dataset_type, dir_name, initial_graph_file), "w") as wf:
        for data in tqdm(initial_graph):
            wf.write("{} {} {}\n".format(data.user, data.item, data.tstamp))

    print("Save Label Data")
    with open(os.path.join(os.getcwd(), "dataset", dataset_type, dir_name, item_label_list_file), "w") as wf:
        wf.write("{}\n".format(max_keyword+1))
        for (item_id, labels) in tqdm(keyword_list):
            wf.write("{} {}\n".format(item_id, ",".join([str(label) for label in sorted(labels)])))

    print("Save Update Stream Data")
    with open(os.path.join(os.getcwd(), "dataset", dataset_type, dir_name, update_stream_file), "w") as wf:
        wf.write("% min:{} max:{}\n".format(min_tstamp, max_tstamp))
        for data in tqdm(update_stream):
            wf.write("{} {} {}\n".format(data.user, data.item, data.tstamp))

    print("Save Keyword Data")
    with open(os.path.join(os.getcwd(), "dataset", dataset_type, dir_name, keyword_file), "w") as wf:
        wf.write("{}\n".format(max_keyword+1))
        for keyword in tqdm(all_keyword_list):
            wf.write("{}\n".format(keyword))
