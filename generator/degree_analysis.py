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
    full_file_path = (os.path.join(os.getcwd(), "dataset", "realworld", file_path))
    raw_data_list = []
    with open(full_file_path, "r") as f:
        raw_data_list = f.readlines()

    # 2. Raw data process
    all_user_degree_list = []
    user_degree_dict = dict()
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

        if user in user_degree_dict:
            user_degree_dict[user] += 1
        else:
            user_degree_dict[user] = 1
        counter += 1
    # print(user_degree_dict)
    max_user_list = sorted(user_degree_dict.items(), key=lambda x: -x[1], reverse=False)
    (max_user, max_value) = max_user_list.pop(0)
    print("%Max:{}, {}".format(max_user, max_value))
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

    analyze_degree("AM", "out.wang-amazon")  # %Max:1433, 44

    analyze_degree("AR", "out.amazon-ratings")  # %Max:10662, 12217

    analyze_degree("BS", "out.bibsonomy-2ui")  # %Max:5065, 428436

    analyze_degree("CM", "out.librec-ciaodvd-movie_ratings")  # %Max:1075, 1106

    analyze_degree("CU", "out.citeulike-ui")  # %Max:19718, 57706

    analyze_degree("DV", "out.digg-votes")  # %Max:364, 10526

    analyze_degree("ML", "out.movielens-10m_ui")  # %Max:615, 6012

    analyze_degree("SX", "out.escorts")  # %Max:2283, 134

    analyze_degree("TA", "out.wang-tripadvisor")  # %Max:6060, 22

    analyze_degree("UF", "out.opsahl-ucforum")  # %Max:213, 1792

    analyze_degree("VU", "out.pics_ti")  # %Max:243, 405429

    analyze_degree("WU", "out.munmun_twitterex_ut")  # %Max:61560, 2431

    pass
