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
    dataset_type = "synthetic-w-4"  # baseline realworld synthetic ablation
    dir_name = "dB/dBkP"
    folder_path = os.path.join(os.getcwd(), folder_path, dataset_type, dir_name)
    print("------{}------".format(folder_path))

    file_list = os.listdir(folder_path)

    for dir_name in file_list:
        dir_path = os.path.join(folder_path, dir_name)
        # if dir_name.startswith("experiment-"):
        if dir_name.startswith(('0', '1', '2', '3', '4', '5', '6', '7', '8', '9', "experiment-")):
            result_file_list = os.listdir(dir_path)
            result_list = []
            for file_name in result_file_list:
                if file_name.startswith("statistic-"):
                    file_path = os.path.join(dir_path, file_name)
                    snapshot_process_time = 0
                    continuous_process_time = 0
                    total_user_node = 0
                    pruning_user_node = 0
                    query_k = 0
                    query_s = 0
                    with open(file_path, encoding='utf-8') as file_obj:
                        lines = file_obj.readlines()
                    for line in lines:
                        if line.startswith("Query Support Threshold:"):
                            query_k = line.rstrip().split(' ')[3]
                        # if line.startswith("Query Keywords File: "):
                        if line.startswith("Sliding Window Size:"):
                            query_s = line.rstrip().split(' ')[3]
                        if line.startswith("Snapshot Query Processing time:"):
                            snapshot_process_time = line.rstrip().split(' ')[4]
                        if line.startswith("Total User Nodes:"):
                            total_user_node = line.rstrip().split(' ')[3]
                        if line.startswith("Pruning Vertices:"):
                            pruning_user_node = line.rstrip().split(' ')[2]
                        if line.startswith("Average Query Process Time:"):
                            continuous_process_time = line.rstrip().split(' ')[4]
                    pruning_ratio = float(pruning_user_node)*100/float(total_user_node)
                    result_list.append([continuous_process_time])
            print(dir_name)
            print(query_k, query_s)
            for uni_result in result_list:
                # print(uni_result[1], sep="\t")
                print(uni_result[0], sep="\t")
