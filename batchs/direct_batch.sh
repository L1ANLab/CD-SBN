cd /home/yons/projects/CD-SBN/

# 

# basic synthetic
build/cdsbn -i dataset/synthetic/dB/dBkL/initial_graph.txt -l dataset/synthetic/dB/dBkL/label_list.txt -u dataset/synthetic/dB/dBkL/update_stream.txt -q dataset/synthetic/dB/dBkL/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic/dP/dPkL/initial_graph.txt -l dataset/synthetic/dP/dPkL/label_list.txt -u dataset/synthetic/dP/dPkL/update_stream.txt -q dataset/synthetic/dP/dPkL/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3

build/cdsbn -i dataset/synthetic/dB/dBkP/initial_graph.txt -l dataset/synthetic/dB/dBkP/label_list.txt -u dataset/synthetic/dB/dBkP/update_stream.txt -q dataset/synthetic/dB/dBkP/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic/dP/dPkP/initial_graph.txt -l dataset/synthetic/dP/dPkP/label_list.txt -u dataset/synthetic/dP/dPkP/update_stream.txt -q dataset/synthetic/dP/dPkP/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3

build/cdsbn -i dataset/synthetic/dB/dBkU/initial_graph.txt -l dataset/synthetic/dB/dBkU/label_list.txt -u dataset/synthetic/dB/dBkU/update_stream.txt -q dataset/synthetic/dB/dBkU/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic/dP/dPkU/initial_graph.txt -l dataset/synthetic/dP/dPkU/label_list.txt -u dataset/synthetic/dP/dPkU/update_stream.txt -q dataset/synthetic/dP/dPkU/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3


# domain: 100, 200, 500, 800, 1000
build/cdsbn -i dataset/synthetic-25k-100/dB/dBkL/initial_graph.txt -l dataset/synthetic-25k-100/dB/dBkL/label_list.txt -u dataset/synthetic-25k-100/dB/dBkL/update_stream.txt -q dataset/synthetic-25k-100/dB/dBkL/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-25k-200/dB/dBkL/initial_graph.txt -l dataset/synthetic-25k-200/dB/dBkL/label_list.txt -u dataset/synthetic-25k-200/dB/dBkL/update_stream.txt -q dataset/synthetic-25k-200/dB/dBkL/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-25k-800/dB/dBkL/initial_graph.txt -l dataset/synthetic-25k-800/dB/dBkL/label_list.txt -u dataset/synthetic-25k-800/dB/dBkL/update_stream.txt -q dataset/synthetic-25k-800/dB/dBkL/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-25k-1000/dB/dBkL/initial_graph.txt -l dataset/synthetic-25k-1000/dB/dBkL/label_list.txt -u dataset/synthetic-25k-1000/dB/dBkL/update_stream.txt -q dataset/synthetic-25k-1000/dB/dBkL/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3

build/cdsbn -i dataset/synthetic-25k-100/dP/dPkL/initial_graph.txt -l dataset/synthetic-25k-100/dP/dPkL/label_list.txt -u dataset/synthetic-25k-100/dP/dPkL/update_stream.txt -q dataset/synthetic-25k-100/dP/dPkL/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-25k-200/dP/dPkL/initial_graph.txt -l dataset/synthetic-25k-200/dP/dPkL/label_list.txt -u dataset/synthetic-25k-200/dP/dPkL/update_stream.txt -q dataset/synthetic-25k-200/dP/dPkL/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-25k-800/dP/dPkL/initial_graph.txt -l dataset/synthetic-25k-800/dP/dPkL/label_list.txt -u dataset/synthetic-25k-800/dP/dPkL/update_stream.txt -q dataset/synthetic-25k-800/dP/dPkL/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-25k-1000/dP/dPkL/initial_graph.txt -l dataset/synthetic-25k-1000/dP/dPkL/label_list.txt -u dataset/synthetic-25k-1000/dP/dPkL/update_stream.txt -q dataset/synthetic-25k-1000/dP/dPkL/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3

build/cdsbn -i dataset/synthetic-25k-100/dB/dBkP/initial_graph.txt -l dataset/synthetic-25k-100/dB/dBkP/label_list.txt -u dataset/synthetic-25k-100/dB/dBkP/update_stream.txt -q dataset/synthetic-25k-100/dB/dBkP/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-25k-200/dB/dBkP/initial_graph.txt -l dataset/synthetic-25k-200/dB/dBkP/label_list.txt -u dataset/synthetic-25k-200/dB/dBkP/update_stream.txt -q dataset/synthetic-25k-200/dB/dBkP/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-25k-800/dB/dBkP/initial_graph.txt -l dataset/synthetic-25k-800/dB/dBkP/label_list.txt -u dataset/synthetic-25k-800/dB/dBkP/update_stream.txt -q dataset/synthetic-25k-800/dB/dBkP/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-25k-1000/dB/dBkP/initial_graph.txt -l dataset/synthetic-25k-1000/dB/dBkP/label_list.txt -u dataset/synthetic-25k-1000/dB/dBkP/update_stream.txt -q dataset/synthetic-25k-1000/dB/dBkP/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3

build/cdsbn -i dataset/synthetic-25k-100/dP/dPkP/initial_graph.txt -l dataset/synthetic-25k-100/dP/dPkP/label_list.txt -u dataset/synthetic-25k-100/dP/dPkP/update_stream.txt -q dataset/synthetic-25k-100/dP/dPkP/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-25k-200/dP/dPkP/initial_graph.txt -l dataset/synthetic-25k-200/dP/dPkP/label_list.txt -u dataset/synthetic-25k-200/dP/dPkP/update_stream.txt -q dataset/synthetic-25k-200/dP/dPkP/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-25k-800/dP/dPkP/initial_graph.txt -l dataset/synthetic-25k-800/dP/dPkP/label_list.txt -u dataset/synthetic-25k-800/dP/dPkP/update_stream.txt -q dataset/synthetic-25k-800/dP/dPkP/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-25k-1000/dP/dPkP/initial_graph.txt -l dataset/synthetic-25k-1000/dP/dPkP/label_list.txt -u dataset/synthetic-25k-1000/dP/dPkP/update_stream.txt -q dataset/synthetic-25k-1000/dP/dPkP/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3

build/cdsbn -i dataset/synthetic-25k-100/dB/dBkU/initial_graph.txt -l dataset/synthetic-25k-100/dB/dBkU/label_list.txt -u dataset/synthetic-25k-100/dB/dBkU/update_stream.txt -q dataset/synthetic-25k-100/dB/dBkU/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-25k-200/dB/dBkU/initial_graph.txt -l dataset/synthetic-25k-200/dB/dBkU/label_list.txt -u dataset/synthetic-25k-200/dB/dBkU/update_stream.txt -q dataset/synthetic-25k-200/dB/dBkU/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-25k-800/dB/dBkU/initial_graph.txt -l dataset/synthetic-25k-800/dB/dBkU/label_list.txt -u dataset/synthetic-25k-800/dB/dBkU/update_stream.txt -q dataset/synthetic-25k-800/dB/dBkU/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-25k-1000/dB/dBkU/initial_graph.txt -l dataset/synthetic-25k-1000/dB/dBkU/label_list.txt -u dataset/synthetic-25k-1000/dB/dBkU/update_stream.txt -q dataset/synthetic-25k-1000/dB/dBkU/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3

build/cdsbn -i dataset/synthetic-25k-100/dP/dPkU/initial_graph.txt -l dataset/synthetic-25k-100/dP/dPkU/label_list.txt -u dataset/synthetic-25k-100/dP/dPkU/update_stream.txt -q dataset/synthetic-25k-100/dP/dPkU/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-25k-200/dP/dPkU/initial_graph.txt -l dataset/synthetic-25k-200/dP/dPkU/label_list.txt -u dataset/synthetic-25k-200/dP/dPkU/update_stream.txt -q dataset/synthetic-25k-200/dP/dPkU/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-25k-800/dP/dPkU/initial_graph.txt -l dataset/synthetic-25k-800/dP/dPkU/label_list.txt -u dataset/synthetic-25k-800/dP/dPkU/update_stream.txt -q dataset/synthetic-25k-800/dP/dPkU/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-25k-1000/dP/dPkU/initial_graph.txt -l dataset/synthetic-25k-1000/dP/dPkU/label_list.txt -u dataset/synthetic-25k-1000/dP/dPkU/update_stream.txt -q dataset/synthetic-25k-1000/dP/dPkU/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3

# keywords per vertex: 1 2 3 4 5
build/cdsbn -i dataset/synthetic-25k-1/dB/dBkL/initial_graph.txt -l dataset/synthetic-25k-1/dB/dBkL/label_list.txt -u dataset/synthetic-25k-1/dB/dBkL/update_stream.txt -q dataset/synthetic-25k-1/dB/dBkL/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-25k-2/dB/dBkL/initial_graph.txt -l dataset/synthetic-25k-2/dB/dBkL/label_list.txt -u dataset/synthetic-25k-2/dB/dBkL/update_stream.txt -q dataset/synthetic-25k-2/dB/dBkL/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-25k-4/dB/dBkL/initial_graph.txt -l dataset/synthetic-25k-4/dB/dBkL/label_list.txt -u dataset/synthetic-25k-4/dB/dBkL/update_stream.txt -q dataset/synthetic-25k-4/dB/dBkL/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-25k-5/dB/dBkL/initial_graph.txt -l dataset/synthetic-25k-5/dB/dBkL/label_list.txt -u dataset/synthetic-25k-5/dB/dBkL/update_stream.txt -q dataset/synthetic-25k-5/dB/dBkL/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3

build/cdsbn -i dataset/synthetic-25k-1/dP/dPkL/initial_graph.txt -l dataset/synthetic-25k-1/dP/dPkL/label_list.txt -u dataset/synthetic-25k-1/dP/dPkL/update_stream.txt -q dataset/synthetic-25k-1/dP/dPkL/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-25k-2/dP/dPkL/initial_graph.txt -l dataset/synthetic-25k-2/dP/dPkL/label_list.txt -u dataset/synthetic-25k-2/dP/dPkL/update_stream.txt -q dataset/synthetic-25k-2/dP/dPkL/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-25k-4/dP/dPkL/initial_graph.txt -l dataset/synthetic-25k-4/dP/dPkL/label_list.txt -u dataset/synthetic-25k-4/dP/dPkL/update_stream.txt -q dataset/synthetic-25k-4/dP/dPkL/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-25k-5/dP/dPkL/initial_graph.txt -l dataset/synthetic-25k-5/dP/dPkL/label_list.txt -u dataset/synthetic-25k-5/dP/dPkL/update_stream.txt -q dataset/synthetic-25k-5/dP/dPkL/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3

build/cdsbn -i dataset/synthetic-25k-1/dB/dBkP/initial_graph.txt -l dataset/synthetic-25k-1/dB/dBkP/label_list.txt -u dataset/synthetic-25k-1/dB/dBkP/update_stream.txt -q dataset/synthetic-25k-1/dB/dBkP/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-25k-2/dB/dBkP/initial_graph.txt -l dataset/synthetic-25k-2/dB/dBkP/label_list.txt -u dataset/synthetic-25k-2/dB/dBkP/update_stream.txt -q dataset/synthetic-25k-2/dB/dBkP/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-25k-4/dB/dBkP/initial_graph.txt -l dataset/synthetic-25k-4/dB/dBkP/label_list.txt -u dataset/synthetic-25k-4/dB/dBkP/update_stream.txt -q dataset/synthetic-25k-4/dB/dBkP/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-25k-5/dB/dBkP/initial_graph.txt -l dataset/synthetic-25k-5/dB/dBkP/label_list.txt -u dataset/synthetic-25k-5/dB/dBkP/update_stream.txt -q dataset/synthetic-25k-5/dB/dBkP/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3

build/cdsbn -i dataset/synthetic-25k-1/dP/dPkP/initial_graph.txt -l dataset/synthetic-25k-1/dP/dPkP/label_list.txt -u dataset/synthetic-25k-1/dP/dPkP/update_stream.txt -q dataset/synthetic-25k-1/dP/dPkP/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-25k-2/dP/dPkP/initial_graph.txt -l dataset/synthetic-25k-2/dP/dPkP/label_list.txt -u dataset/synthetic-25k-2/dP/dPkP/update_stream.txt -q dataset/synthetic-25k-2/dP/dPkP/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-25k-4/dP/dPkP/initial_graph.txt -l dataset/synthetic-25k-4/dP/dPkP/label_list.txt -u dataset/synthetic-25k-4/dP/dPkP/update_stream.txt -q dataset/synthetic-25k-4/dP/dPkP/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-25k-5/dP/dPkP/initial_graph.txt -l dataset/synthetic-25k-5/dP/dPkP/label_list.txt -u dataset/synthetic-25k-5/dP/dPkP/update_stream.txt -q dataset/synthetic-25k-5/dP/dPkP/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3

build/cdsbn -i dataset/synthetic-25k-1/dB/dBkU/initial_graph.txt -l dataset/synthetic-25k-1/dB/dBkU/label_list.txt -u dataset/synthetic-25k-1/dB/dBkU/update_stream.txt -q dataset/synthetic-25k-1/dB/dBkU/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-25k-2/dB/dBkU/initial_graph.txt -l dataset/synthetic-25k-2/dB/dBkU/label_list.txt -u dataset/synthetic-25k-2/dB/dBkU/update_stream.txt -q dataset/synthetic-25k-2/dB/dBkU/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-25k-4/dB/dBkU/initial_graph.txt -l dataset/synthetic-25k-4/dB/dBkU/label_list.txt -u dataset/synthetic-25k-4/dB/dBkU/update_stream.txt -q dataset/synthetic-25k-4/dB/dBkU/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-25k-5/dB/dBkU/initial_graph.txt -l dataset/synthetic-25k-5/dB/dBkU/label_list.txt -u dataset/synthetic-25k-5/dB/dBkU/update_stream.txt -q dataset/synthetic-25k-5/dB/dBkU/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3

build/cdsbn -i dataset/synthetic-25k-1/dP/dPkU/initial_graph.txt -l dataset/synthetic-25k-1/dP/dPkU/label_list.txt -u dataset/synthetic-25k-1/dP/dPkU/update_stream.txt -q dataset/synthetic-25k-1/dP/dPkU/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-25k-2/dP/dPkU/initial_graph.txt -l dataset/synthetic-25k-2/dP/dPkU/label_list.txt -u dataset/synthetic-25k-2/dP/dPkU/update_stream.txt -q dataset/synthetic-25k-2/dP/dPkU/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-25k-4/dP/dPkU/initial_graph.txt -l dataset/synthetic-25k-4/dP/dPkU/label_list.txt -u dataset/synthetic-25k-4/dP/dPkU/update_stream.txt -q dataset/synthetic-25k-4/dP/dPkU/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-25k-5/dP/dPkU/initial_graph.txt -l dataset/synthetic-25k-5/dP/dPkU/label_list.txt -u dataset/synthetic-25k-5/dP/dPkU/update_stream.txt -q dataset/synthetic-25k-5/dP/dPkU/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3


# Size of L(G): 10K, 15K, 25K, 50K, 100K
build/cdsbn -i dataset/synthetic-L10k/dB/dBkL/initial_graph.txt -l dataset/synthetic-L10k/dB/dBkL/label_list.txt -u dataset/synthetic-L10k/dB/dBkL/update_stream.txt -q dataset/synthetic-L10k/dB/dBkL/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-L10k/dP/dPkL/initial_graph.txt -l dataset/synthetic-L10k/dP/dPkL/label_list.txt -u dataset/synthetic-L10k/dP/dPkL/update_stream.txt -q dataset/synthetic-L10k/dP/dPkL/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-L10k/dB/dBkP/initial_graph.txt -l dataset/synthetic-L10k/dB/dBkP/label_list.txt -u dataset/synthetic-L10k/dB/dBkP/update_stream.txt -q dataset/synthetic-L10k/dB/dBkP/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-L10k/dP/dPkP/initial_graph.txt -l dataset/synthetic-L10k/dP/dPkP/label_list.txt -u dataset/synthetic-L10k/dP/dPkP/update_stream.txt -q dataset/synthetic-L10k/dP/dPkP/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-L10k/dB/dBkU/initial_graph.txt -l dataset/synthetic-L10k/dB/dBkU/label_list.txt -u dataset/synthetic-L10k/dB/dBkU/update_stream.txt -q dataset/synthetic-L10k/dB/dBkU/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-L10k/dP/dPkU/initial_graph.txt -l dataset/synthetic-L10k/dP/dPkU/label_list.txt -u dataset/synthetic-L10k/dP/dPkU/update_stream.txt -q dataset/synthetic-L10k/dP/dPkU/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3

build/cdsbn -i dataset/synthetic-L15k/dB/dBkL/initial_graph.txt -l dataset/synthetic-L15k/dB/dBkL/label_list.txt -u dataset/synthetic-L15k/dB/dBkL/update_stream.txt -q dataset/synthetic-L15k/dB/dBkL/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-L15k/dP/dPkL/initial_graph.txt -l dataset/synthetic-L15k/dP/dPkL/label_list.txt -u dataset/synthetic-L15k/dP/dPkL/update_stream.txt -q dataset/synthetic-L15k/dP/dPkL/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-L15k/dB/dBkP/initial_graph.txt -l dataset/synthetic-L15k/dB/dBkP/label_list.txt -u dataset/synthetic-L15k/dB/dBkP/update_stream.txt -q dataset/synthetic-L15k/dB/dBkP/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-L15k/dP/dPkP/initial_graph.txt -l dataset/synthetic-L15k/dP/dPkP/label_list.txt -u dataset/synthetic-L15k/dP/dPkP/update_stream.txt -q dataset/synthetic-L15k/dP/dPkP/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-L15k/dB/dBkU/initial_graph.txt -l dataset/synthetic-L15k/dB/dBkU/label_list.txt -u dataset/synthetic-L15k/dB/dBkU/update_stream.txt -q dataset/synthetic-L15k/dB/dBkU/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-L15k/dP/dPkU/initial_graph.txt -l dataset/synthetic-L15k/dP/dPkU/label_list.txt -u dataset/synthetic-L15k/dP/dPkU/update_stream.txt -q dataset/synthetic-L15k/dP/dPkU/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3

build/cdsbn -i dataset/synthetic-L50k/dB/dBkL/initial_graph.txt -l dataset/synthetic-L50k/dB/dBkL/label_list.txt -u dataset/synthetic-L50k/dB/dBkL/update_stream.txt -q dataset/synthetic-L50k/dB/dBkL/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-L50k/dP/dPkL/initial_graph.txt -l dataset/synthetic-L50k/dP/dPkL/label_list.txt -u dataset/synthetic-L50k/dP/dPkL/update_stream.txt -q dataset/synthetic-L50k/dP/dPkL/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-L50k/dB/dBkP/initial_graph.txt -l dataset/synthetic-L50k/dB/dBkP/label_list.txt -u dataset/synthetic-L50k/dB/dBkP/update_stream.txt -q dataset/synthetic-L50k/dB/dBkP/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-L50k/dP/dPkP/initial_graph.txt -l dataset/synthetic-L50k/dP/dPkP/label_list.txt -u dataset/synthetic-L50k/dP/dPkP/update_stream.txt -q dataset/synthetic-L50k/dP/dPkP/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-L50k/dB/dBkU/initial_graph.txt -l dataset/synthetic-L50k/dB/dBkU/label_list.txt -u dataset/synthetic-L50k/dB/dBkU/update_stream.txt -q dataset/synthetic-L50k/dB/dBkU/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-L50k/dP/dPkU/initial_graph.txt -l dataset/synthetic-L50k/dP/dPkU/label_list.txt -u dataset/synthetic-L50k/dP/dPkU/update_stream.txt -q dataset/synthetic-L50k/dP/dPkU/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3

build/cdsbn -i dataset/synthetic-L100k/dB/dBkL/initial_graph.txt -l dataset/synthetic-L100k/dB/dBkL/label_list.txt -u dataset/synthetic-L100k/dB/dBkL/update_stream.txt -q dataset/synthetic-L100k/dB/dBkL/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-L100k/dP/dPkL/initial_graph.txt -l dataset/synthetic-L100k/dP/dPkL/label_list.txt -u dataset/synthetic-L100k/dP/dPkL/update_stream.txt -q dataset/synthetic-L100k/dP/dPkL/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-L100k/dB/dBkP/initial_graph.txt -l dataset/synthetic-L100k/dB/dBkP/label_list.txt -u dataset/synthetic-L100k/dB/dBkP/update_stream.txt -q dataset/synthetic-L100k/dB/dBkP/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-L100k/dP/dPkP/initial_graph.txt -l dataset/synthetic-L100k/dP/dPkP/label_list.txt -u dataset/synthetic-L100k/dP/dPkP/update_stream.txt -q dataset/synthetic-L100k/dP/dPkP/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-L100k/dB/dBkU/initial_graph.txt -l dataset/synthetic-L100k/dB/dBkU/label_list.txt -u dataset/synthetic-L100k/dB/dBkU/update_stream.txt -q dataset/synthetic-L100k/dB/dBkU/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-L100k/dP/dPkU/initial_graph.txt -l dataset/synthetic-L100k/dP/dPkU/label_list.txt -u dataset/synthetic-L100k/dP/dPkU/update_stream.txt -q dataset/synthetic-L100k/dP/dPkU/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3


# Size of U(G): 10K, 15K, 25K, 50K, 100K
build/cdsbn -i dataset/synthetic-U10k/dB/dBkL/initial_graph.txt -l dataset/synthetic-U10k/dB/dBkL/label_list.txt -u dataset/synthetic-U10k/dB/dBkL/update_stream.txt -q dataset/synthetic-U10k/dB/dBkL/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-U10k/dP/dPkL/initial_graph.txt -l dataset/synthetic-U10k/dP/dPkL/label_list.txt -u dataset/synthetic-U10k/dP/dPkL/update_stream.txt -q dataset/synthetic-U10k/dP/dPkL/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-U10k/dB/dBkP/initial_graph.txt -l dataset/synthetic-U10k/dB/dBkP/label_list.txt -u dataset/synthetic-U10k/dB/dBkP/update_stream.txt -q dataset/synthetic-U10k/dB/dBkP/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-U10k/dP/dPkP/initial_graph.txt -l dataset/synthetic-U10k/dP/dPkP/label_list.txt -u dataset/synthetic-U10k/dP/dPkP/update_stream.txt -q dataset/synthetic-U10k/dP/dPkP/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-U10k/dB/dBkU/initial_graph.txt -l dataset/synthetic-U10k/dB/dBkU/label_list.txt -u dataset/synthetic-U10k/dB/dBkU/update_stream.txt -q dataset/synthetic-U10k/dB/dBkU/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-U10k/dP/dPkU/initial_graph.txt -l dataset/synthetic-U10k/dP/dPkU/label_list.txt -u dataset/synthetic-U10k/dP/dPkU/update_stream.txt -q dataset/synthetic-U10k/dP/dPkU/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3

build/cdsbn -i dataset/synthetic-U15k/dB/dBkL/initial_graph.txt -l dataset/synthetic-U15k/dB/dBkL/label_list.txt -u dataset/synthetic-U15k/dB/dBkL/update_stream.txt -q dataset/synthetic-U15k/dB/dBkL/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-U15k/dP/dPkL/initial_graph.txt -l dataset/synthetic-U15k/dP/dPkL/label_list.txt -u dataset/synthetic-U15k/dP/dPkL/update_stream.txt -q dataset/synthetic-U15k/dP/dPkL/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-U15k/dB/dBkP/initial_graph.txt -l dataset/synthetic-U15k/dB/dBkP/label_list.txt -u dataset/synthetic-U15k/dB/dBkP/update_stream.txt -q dataset/synthetic-U15k/dB/dBkP/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-U15k/dP/dPkP/initial_graph.txt -l dataset/synthetic-U15k/dP/dPkP/label_list.txt -u dataset/synthetic-U15k/dP/dPkP/update_stream.txt -q dataset/synthetic-U15k/dP/dPkP/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-U15k/dB/dBkU/initial_graph.txt -l dataset/synthetic-U15k/dB/dBkU/label_list.txt -u dataset/synthetic-U15k/dB/dBkU/update_stream.txt -q dataset/synthetic-U15k/dB/dBkU/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-U15k/dP/dPkU/initial_graph.txt -l dataset/synthetic-U15k/dP/dPkU/label_list.txt -u dataset/synthetic-U15k/dP/dPkU/update_stream.txt -q dataset/synthetic-U15k/dP/dPkU/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3

build/cdsbn -i dataset/synthetic-U50k/dB/dBkL/initial_graph.txt -l dataset/synthetic-U50k/dB/dBkL/label_list.txt -u dataset/synthetic-U50k/dB/dBkL/update_stream.txt -q dataset/synthetic-U50k/dB/dBkL/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-U50k/dP/dPkL/initial_graph.txt -l dataset/synthetic-U50k/dP/dPkL/label_list.txt -u dataset/synthetic-U50k/dP/dPkL/update_stream.txt -q dataset/synthetic-U50k/dP/dPkL/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-U50k/dB/dBkP/initial_graph.txt -l dataset/synthetic-U50k/dB/dBkP/label_list.txt -u dataset/synthetic-U50k/dB/dBkP/update_stream.txt -q dataset/synthetic-U50k/dB/dBkP/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-U50k/dP/dPkP/initial_graph.txt -l dataset/synthetic-U50k/dP/dPkP/label_list.txt -u dataset/synthetic-U50k/dP/dPkP/update_stream.txt -q dataset/synthetic-U50k/dP/dPkP/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-U50k/dB/dBkU/initial_graph.txt -l dataset/synthetic-U50k/dB/dBkU/label_list.txt -u dataset/synthetic-U50k/dB/dBkU/update_stream.txt -q dataset/synthetic-U50k/dB/dBkU/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-U50k/dP/dPkU/initial_graph.txt -l dataset/synthetic-U50k/dP/dPkU/label_list.txt -u dataset/synthetic-U50k/dP/dPkU/update_stream.txt -q dataset/synthetic-U50k/dP/dPkU/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3

build/cdsbn -i dataset/synthetic-U100k/dB/dBkL/initial_graph.txt -l dataset/synthetic-U100k/dB/dBkL/label_list.txt -u dataset/synthetic-U100k/dB/dBkL/update_stream.txt -q dataset/synthetic-U100k/dB/dBkL/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-U100k/dP/dPkL/initial_graph.txt -l dataset/synthetic-U100k/dP/dPkL/label_list.txt -u dataset/synthetic-U100k/dP/dPkL/update_stream.txt -q dataset/synthetic-U100k/dP/dPkL/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-U100k/dB/dBkP/initial_graph.txt -l dataset/synthetic-U100k/dB/dBkP/label_list.txt -u dataset/synthetic-U100k/dB/dBkP/update_stream.txt -q dataset/synthetic-U100k/dB/dBkP/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-U100k/dP/dPkP/initial_graph.txt -l dataset/synthetic-U100k/dP/dPkP/label_list.txt -u dataset/synthetic-U100k/dP/dPkP/update_stream.txt -q dataset/synthetic-U100k/dP/dPkP/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-U100k/dB/dBkU/initial_graph.txt -l dataset/synthetic-U100k/dB/dBkU/label_list.txt -u dataset/synthetic-U100k/dB/dBkU/update_stream.txt -q dataset/synthetic-U100k/dB/dBkU/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-U100k/dP/dPkU/initial_graph.txt -l dataset/synthetic-U100k/dP/dPkU/label_list.txt -u dataset/synthetic-U100k/dP/dPkU/update_stream.txt -q dataset/synthetic-U100k/dP/dPkU/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
