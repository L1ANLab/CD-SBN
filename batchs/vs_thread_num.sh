cd /home/yons/projects/CD-SBN/
# 10 20 50 80 100
build/cdsbn -i dataset/synthetic-thread-num/dB/dBkL/initial_graph.txt -l dataset/synthetic-thread-num/dB/dBkL/label_list.txt -u dataset/synthetic-thread-num/dB/dBkL/update_stream.txt -q dataset/synthetic-thread-num/dB/dBkL/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-thread-num/dB/dBkU/initial_graph.txt -l dataset/synthetic-thread-num/dB/dBkU/label_list.txt -u dataset/synthetic-thread-num/dB/dBkU/update_stream.txt -q dataset/synthetic-thread-num/dB/dBkU/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-thread-num/dB/dBkP/initial_graph.txt -l dataset/synthetic-thread-num/dB/dBkP/label_list.txt -u dataset/synthetic-thread-num/dB/dBkP/update_stream.txt -q dataset/synthetic-thread-num/dB/dBkP/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3

build/cdsbn -i dataset/synthetic-thread-num/dP/dPkL/initial_graph.txt -l dataset/synthetic-thread-num/dP/dPkL/label_list.txt -u dataset/synthetic-thread-num/dP/dPkL/update_stream.txt -q dataset/synthetic-thread-num/dP/dPkL/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-thread-num/dP/dPkU/initial_graph.txt -l dataset/synthetic-thread-num/dP/dPkU/label_list.txt -u dataset/synthetic-thread-num/dP/dPkU/update_stream.txt -q dataset/synthetic-thread-num/dP/dPkU/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-thread-num/dP/dPkP/initial_graph.txt -l dataset/synthetic-thread-num/dP/dPkP/label_list.txt -u dataset/synthetic-thread-num/dP/dPkP/update_stream.txt -q dataset/synthetic-thread-num/dP/dPkP/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
