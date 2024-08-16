import os

from matplotlib import pyplot as plt
from distfit import distfit
import numpy as np
from tqdm import tqdm

# ALL_KEYWORD_NUM = 20
# KEYWORDS_PER_VERTEX_NUM = 3
INITIAL_GRAPH_RATIO = 0.9999


def analyze_keywords(dir_name: str, keyword_file_name: str):
    print("Process [{}]".format(dir_name))
    # 0. Param settings
    keyword_file_path = os.path.join(dir_name, keyword_file_name)

    # 4. read the keyword file
    full_keyword_file_path = (os.path.join(os.getcwd(), "dataset", keyword_file_path))
    raw_keyword_data_list = []
    with open(full_keyword_file_path, "r") as f:
        raw_keyword_data_list = f.readlines()

    # 5. keyword data process
    max_keyword = 0
    keyword_dict = dict()
    all_keyword_freq_dict = dict()
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

        if item not in keyword_dict:
            keyword_dict[item] = [keyword]
        elif keyword not in keyword_dict[item]:
            keyword_dict[item].append(keyword)

        if keyword not in all_keyword_freq_dict:
            all_keyword_freq_dict[keyword] = 1
        else:
            all_keyword_freq_dict[keyword] += 1

        all_keyword_list.append(keyword)
        if keyword > max_keyword:
            max_keyword = keyword
        counter += 1
    # keyword_list = sorted(keyword_list.items(), key=lambda d: int(d[0]), reverse=False)

    # item_keywords_dict = dict()
    # item_keywords_dict["item_id"] = keyword_dict.keys()
    # item_keywords_dict["keywords"] = keyword_dict.items()
    print(len(all_keyword_freq_dict))

    all_keyword_list = np.array(all_keyword_list)

    # plt.switch_backend('agg')
    # plt.hist(all_keyword_list)
    # plt.savefig("./{}_keyword_vis".format(dir_name))

    # keyword_df = pd.DataFrame(all_keyword_list, columns=['Keyword'])
    # print(keyword_df.describe())
    distr_list = ['norm', "genextreme", "expon", "gamma", 'pareto',
                  "lognorm", "dweibull", "beta", "t", "uniform", "laplace",
                  "logistic", "maxwell", "powerlaw", "powerlognorm", "powernorm"]
    dfit = distfit(todf=True, distr=distr_list)
    dfit.fit_transform(all_keyword_list)
    dfit.plot()
    fig_path = os.path.join(os.getcwd(), "dataset", dir_name, "./{}_keyword_vis".format(dir_name))
    plt.savefig(fig_path)

    dfit.plot_summary()
    fig_path = os.path.join(os.getcwd(), "dataset", dir_name, "./{}_keyword_vis_summary".format(dir_name))
    plt.savefig(fig_path)

    output_path = os.path.join(os.getcwd(), "dataset", dir_name, "keyword_distribution_fit.csv")
    dfit.summary.to_csv(output_path, sep=',', index=True, header=True)


if __name__ == "__main__":

    analyze_keywords("BS", "out.bibsonomy-2ti")

    analyze_keywords("CU", "out.citeulike-ti")

    analyze_keywords("ML", "out.movielens-10m_ti")

    analyze_keywords("VU", "out.pics_ti")

    pass
