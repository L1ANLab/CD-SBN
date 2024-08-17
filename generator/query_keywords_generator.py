import os
import numpy as np
from tqdm import tqdm
# from fitter import fitter


def generate_query_keywords(dataset_type: str, dir_name: str, file_name: str, query_keywords_size: int, query_keywords_num: int):
    # 0. Param settings
    keyword_file_path = os.path.join(dataset_type, dir_name, file_name)

    # 1. keyword file read
    full_file_path = (os.path.join(os.getcwd(), "dataset", keyword_file_path))
    raw_keyword_data_list = []
    with open(full_file_path, "r") as f:
        total_num = f.readline()
        raw_keyword_data_list = f.readlines()
    total_num = int(total_num)
    # 2. process keyword data
    keywords_list = [i for i in range(total_num)]
    keywords_count_list = [0 for _ in range(total_num)]
    for line in tqdm(raw_keyword_data_list):
        if line.startswith("%"):
            continue
        keyword = int(line.strip().split(" ").pop())
        keywords_count_list[keyword] += 1
    keywords_list = sorted(keywords_list, reverse=False)
    keywords_count_list = np.array(keywords_count_list)

    # 3. Generate query keywords
    query_keywords_list = []
    for _ in range(query_keywords_num):
        query_keywords = np.random.choice(
            keywords_list,
            size=query_keywords_size,
            p=keywords_count_list/sum(keywords_count_list),
            replace=False
        )
        query_keywords_list.append(query_keywords.tolist())
        # print(query_keywords.tolist())

    # 4. save the query keywords
    query_keyword_file = "query_keywords_list-{}.txt".format(query_keywords_size)
    with open(os.path.join(os.getcwd(), "dataset", dataset_type, dir_name, query_keyword_file), "w") as wf:
        wf.write("{} {}\n".format(query_keywords_num, query_keywords_size))
        for query_keywords in tqdm(query_keywords_list):
            wf.write("{}\n".format(" ".join(map(str, query_keywords))))


if __name__ == "__main__":
    # generate_query_keywords("AM", "keywords_list.txt", 5, 5)
    # generate_query_keywords("AR", "keywords_list.txt", 5, 5)
    # generate_query_keywords("BS", "keywords_list.txt", 5, 5)
    # generate_query_keywords("CM", "keywords_list.txt", 5, 5)
    # generate_query_keywords("CU", "keywords_list.txt", 5, 5)
    # generate_query_keywords("DV", "keywords_list.txt", 5, 5)
    # generate_query_keywords("ML", "keywords_list.txt", 5, 5)
    # generate_query_keywords("SX", "keywords_list.txt", 5, 5)
    # generate_query_keywords("TA", "keywords_list.txt", 5, 5)
    # generate_query_keywords("UF", "keywords_list.txt", 5, 5)
    # generate_query_keywords("VU", "keywords_list.txt", 5, 5)
    # generate_query_keywords("WU", "keywords_list.txt", 5, 5)

    # generate_query_keywords("PL", "keywords_list.txt", 5, 5)
    # generate_query_keywords("UD", "keywords_list.txt", 5, 5)
    pass
