import os

if __name__ == "__main__":
    # folder_path = "/home/yons/projects/influential-community-detection/dataset/"
    folder_path = "dataset"
    # "AM"
    # "AR"
    # "BS"
    # "CM"
    # "CU"
    # "DV"
    # "ML"
    # "SX"
    # "TA"
    # "UF"
    # "VU"
    # "WU"
    # "dB/dBkL"
    # "dB/dBkP"
    # "dB/dBkU"
    # "dP/dPkL"
    # "dP/dPkP"
    # "dP/dPkU"
    dataset_type = "realworld"  # baseline realworld synthetic
    dir_name = "SX"
    folder_path = os.path.join(os.getcwd(), folder_path, dataset_type, dir_name)
    print("------{}------".format(folder_path))

    file_list = os.listdir(folder_path)

    for dir_name in file_list:
        dir_path = os.path.join(folder_path, dir_name)
        if dir_name.startswith("experiment-"):
            result_file_list = os.listdir(dir_path)
            result_list = []
            for file_name in result_file_list:
                if file_name.startswith("statistic-"):
                    file_path = os.path.join(dir_path, file_name)
                    snapshot_process_time = 0
                    continuous_process_time = 0
                    query_k = 0
                    query_s = 0
                    with open(file_path, encoding='utf-8') as file_obj:
                        lines = file_obj.readlines()
                    for line in lines:
                        if line.startswith("Query Support Threshold:"):
                            query_k = line.rstrip().split(' ')[3]
                        if line.startswith("Sliding Window Size:"):
                            query_s = line.rstrip().split(' ')[3]
                        if line.startswith("Snapshot Query Processing time:"):
                            snapshot_process_time = line.rstrip().split(' ')[4]
                        if line.startswith("Average Query Process Time:"):
                            continuous_process_time = line.rstrip().split(' ')[4]
                    # refine_time = float(refine_time) * (float(sampling_ratio) if float(sampling_ratio) > 1 else 1)
                    result_list.append([snapshot_process_time, continuous_process_time])
            print(dir_name)
            # print(query_k, query_s)
            for uni_result in result_list:
                print(uni_result[1], sep="\t")
