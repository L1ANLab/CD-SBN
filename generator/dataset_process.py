import os

import numpy as np
from scipy.stats import zipf
from tqdm import tqdm

ALL_KEYWORD_NUM = 20
KEYWORDS_PER_VERTEX_NUM = 3


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


if __name__ == "__main__":
    # 0. Param settings
    dir_name = "BibSonomy(BS)"
    file_path = os.path.join(dir_name, "bibsonomy-2ui/out.bibsonomy-2ui")
    distribution = "uniform"
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

    # 3. input file generate
    initial_graph_size = int(len(data_list) * 0.2)
    initial_graph = data_list[:initial_graph_size]

    update_stream = data_list[initial_graph_size:]

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

    label_counter = [0 for _ in range(all_keyword_num)]
    for item_id in range(max_item+1):
        keyword_num = np.random.randint(max(keywords_per_vertex_num-1, 1), keywords_per_vertex_num+2)  # the num is key_per ± 1

        keywords = np.random.choice(keywords_set, keyword_num)
        while len(set(keywords)) != len(keywords):
            keywords = np.random.choice(keywords_set, keyword_num)
        for keyword in keywords:
            label_counter[keyword] += 1
        item_label_list.append(keywords)

    initial_graph_file = "initial_graph.txt"
    item_label_list_file = "label_list.txt"
    update_stream_file = "update_stream.txt"
    with open(os.path.join(os.getcwd(), "dataset", dir_name, initial_graph_file), "w") as wf:
        for data in tqdm(initial_graph):
            wf.write("{} {}\n".format(data.user, data.item))

    with open(os.path.join(os.getcwd(), "dataset", dir_name, item_label_list_file), "w") as wf:
        for item_id, labels in tqdm(enumerate(item_label_list)):
            wf.write("{} {}\n".format(item_id, ",".join([str(label) for label in labels])))

    with open(os.path.join(os.getcwd(), "dataset", dir_name, update_stream_file), "w") as wf:
        wf.write("% {} {}\n".format(min_tstamp, max_tstamp))
        for data in tqdm(update_stream):
            wf.write("{} {} {}\n".format(data.user, data.item, data.tstamp))
