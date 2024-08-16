import os

from matplotlib import pyplot as plt
from distfit import distfit
import numpy as np
from tqdm import tqdm


def analyze_degree(dir_name: str, graph_file_name: str):
    print("Process [{}]".format(dir_name))
    # 0. Param settings
    file_path = os.path.join(dir_name, graph_file_name)

    # 1. File read
    full_file_path = (os.path.join(os.getcwd(), "dataset", file_path))
    raw_data_list = []
    with open(full_file_path, "r") as f:
        raw_data_list = f.readlines()

    # 2. Raw data process
    all_user_degree_list = []
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

        all_user_degree_list.append(user)

        counter += 1

    all_user_degree_list = np.array(all_user_degree_list)

    # plt.switch_backend('agg')
    # plt.hist(all_keyword_list)
    # plt.savefig("./{}_keyword_vis".format(dir_name))

    # keyword_df = pd.DataFrame(all_keyword_list, columns=['Keyword'])
    # print(keyword_df.describe())
    distr_list = ['norm', "genextreme", "expon", "gamma", 'pareto',
                  "lognorm", "dweibull", "beta", "t", "uniform", "laplace",
                  "logistic", "maxwell", "powerlaw", "powerlognorm", "powernorm"]
    dfit = distfit(todf=True, distr=distr_list)
    dfit.fit_transform(all_user_degree_list)
    dfit.plot()
    fig_path = os.path.join(os.getcwd(), "dataset", dir_name, "./{}_user_degree_vis".format(dir_name))
    plt.savefig(fig_path)

    dfit.plot_summary()
    fig_path = os.path.join(os.getcwd(), "dataset", dir_name, "./{}_user_degree_vis_summary".format(dir_name))
    plt.savefig(fig_path)

    output_path = os.path.join(os.getcwd(), "dataset", dir_name, "user_degree_distribution_fit.csv")
    dfit.summary.to_csv(output_path, sep=',', index=True, header=True)


if __name__ == "__main__":

    analyze_degree("AM", "out.wang-amazon")

    analyze_degree("AR", "out.amazon-ratings")

    analyze_degree("BS", "out.bibsonomy-2ui")

    analyze_degree("CM", "out.librec-ciaodvd-movie_ratings")

    analyze_degree("CU", "out.citeulike-ui")

    analyze_degree("DV", "out.digg-votes")

    analyze_degree("ML", "out.movielens-10m_ui")

    analyze_degree("SX", "out.escorts")

    analyze_degree("TA", "out.wang-tripadvisor")

    analyze_degree("UF", "out.opsahl-ucforum")

    analyze_degree("VU", "out.pics_ti")

    analyze_degree("WU", "out.munmun_twitterex_ut")

    pass
