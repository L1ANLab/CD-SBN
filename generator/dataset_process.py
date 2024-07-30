import os

import numpy as np
from scipy.stats import zipf
from tqdm import tqdm

ALL_KEYWORD_NUM = 20
KEYWORDS_PER_VERTEX_NUM = 3
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


def GenerateDataset(dir_name: str, file_name: str, distribution: str):
    # 0. Param settings
    file_path = os.path.join(dir_name, file_name)
    all_keyword_num = ALL_KEYWORD_NUM
    keywords_per_vertex_num = KEYWORDS_PER_VERTEX_NUM

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
    for line in tqdm(raw_data_list):
        if line.startswith("%"):
            continue
        [user, item, weight, tstamp] = line.strip().split(" ")
        for _ in range(int(weight)):
            now_data = EdgeRawData(int(user), int(item), int(weight), int(tstamp))
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

    data_list = sorted(data_list)

    # 3. divide inital graph and update stream
    initial_graph_size = int(len(data_list) * INITIAL_GRAPH_RATIO)
    initial_graph = data_list[:initial_graph_size]
    update_stream = data_list[initial_graph_size:]

    # 4. generate distribution sample
    item_label_list = []
    if distribution == "uniform":
        keywords_set = range(0, all_keyword_num)
    elif distribution == "zipf":
        # Zipf
        a = 2  # param
        zipf_dist = zipf(a)
        size = max_item + 1
        keywords_set = zipf_dist.rvs(size * keywords_per_vertex_num)
    elif distribution == "gauss":
        # Gaussian
        mean = 10
        stddev = 3
        if all_keyword_num == 10:
            mean = 5
            stddev = 1.5
        elif all_keyword_num == 20:
            mean = 10
            stddev = 3
        elif all_keyword_num == 50:
            mean = 25
            stddev = 7.5
        elif all_keyword_num == 80:
            mean = 40
            stddev = 12
        size = max_item + 1
        keywords_set = np.random.normal(mean, stddev, size)
    keywords_set = np.clip(keywords_set, 0, all_keyword_num-1).astype(int)

    # 5. generate keywords following distribution
    label_counter = [0 for _ in range(all_keyword_num)]
    for item_id in range(max_item+1):
        keyword_num = np.random.randint(max(keywords_per_vertex_num-1, 1), keywords_per_vertex_num+2)  # the num is key_per ± 1

        keywords = np.random.choice(keywords_set, keyword_num)
        while len(set(keywords)) != len(keywords):
            keywords = np.random.choice(keywords_set, keyword_num)
        for keyword in keywords:
            label_counter[keyword] += 1
        item_label_list.append(keywords)

    # 6. write dataset into files
    initial_graph_file = "initial_graph.txt"
    item_label_list_file = "label_list.txt"
    update_stream_file = "update_stream.txt"
    with open(os.path.join(os.getcwd(), "dataset", dir_name, initial_graph_file), "w") as wf:
        for data in tqdm(initial_graph):
            wf.write("{} {} {}\n".format(data.user, data.item, data.tstamp))

    with open(os.path.join(os.getcwd(), "dataset", dir_name, item_label_list_file), "w") as wf:
        wf.write("{}\n".format(ALL_KEYWORD_NUM))
        for item_id, labels in tqdm(enumerate(item_label_list)):
            wf.write("{} {}\n".format(item_id, ",".join([str(label) for label in labels])))

    with open(os.path.join(os.getcwd(), "dataset", dir_name, update_stream_file), "w") as wf:
        wf.write("% min:{} max:{}\n".format(min_tstamp, max_tstamp))
        for data in tqdm(update_stream):
            wf.write("{} {} {}\n".format(data.user, data.item, data.tstamp))


def GenerateDatasetWithKeywords(dir_name: str, graph_file_name: str, keyword_file_name: str):
    # 0. Param settings
    graph_file_path = os.path.join(dir_name, graph_file_name)
    keyword_file_path = os.path.join(dir_name, keyword_file_name)

    # 1. Graph file read
    full_file_path = (os.path.join(os.getcwd(), "dataset", graph_file_path))
    raw_data_list = []
    with open(full_file_path, "r") as f:
        raw_data_list = f.readlines()

    # 2. Raw data process
    max_item = 0
    min_tstamp = float("inf")
    max_tstamp = 0

    data_list = []
    for line in tqdm(raw_data_list):
        if line.startswith("%"):
            continue
        [user, item, weight, tstamp] = line.strip().split(" ")
        for _ in range(int(weight)):
            now_data = EdgeRawData(int(user), int(item), int(weight), int(tstamp))
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
    for line in tqdm(raw_keyword_data_list):
        if line.startswith("%"):
            continue
        [keyword, item, weight, tstamp] = line.strip().split(" ")
        keyword = int(keyword)
        if item not in keyword_list:
            keyword_list[item] = [keyword]
        elif keyword not in keyword_list[item]:
            keyword_list[item].append(keyword)
        all_keyword_list.append(keyword)

        if keyword > max_keyword:
            max_keyword = keyword
    keyword_list = sorted(keyword_list.items(), key=lambda d: int(d[0]), reverse=False)

    # 6. write dataset into files
    initial_graph_file = "initial_graph.txt"
    item_label_list_file = "label_list.txt"
    update_stream_file = "update_stream.txt"
    keyword_file = "keywords_list.txt"
    with open(os.path.join(os.getcwd(), "dataset", dir_name, initial_graph_file), "w") as wf:
        for data in tqdm(initial_graph):
            wf.write("{} {} {}\n".format(data.user, data.item, data.tstamp))

    with open(os.path.join(os.getcwd(), "dataset", dir_name, item_label_list_file), "w") as wf:
        wf.write("{}\n".format(max_keyword+1))
        for (item_id, labels) in tqdm(keyword_list):
            wf.write("{} {}\n".format(item_id, ",".join([str(label) for label in sorted(labels)])))

    with open(os.path.join(os.getcwd(), "dataset", dir_name, update_stream_file), "w") as wf:
        wf.write("% min:{} max:{}\n".format(min_tstamp, max_tstamp))
        for data in tqdm(update_stream):
            wf.write("{} {} {}\n".format(data.user, data.item, data.tstamp))

    with open(os.path.join(os.getcwd(), "dataset", dir_name, keyword_file), "w") as wf:
        wf.write("{}\n".format(max_keyword+1))
        for keyword in tqdm(all_keyword_list):
            wf.write("{}\n".format(keyword))


if __name__ == "__main__":
    # GenerateDataset("BS", "out.bibsonomy-2ui", "uniform")

    GenerateDatasetWithKeywords("BS", "out.bibsonomy-2ui", "out.bibsonomy-2ti")
