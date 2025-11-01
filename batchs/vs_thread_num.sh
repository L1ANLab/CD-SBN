cd /home/yons/projects/CD-SBN/
# 10 20 50 80 100 120 150
build/cdsbn -i dataset/synthetic-thread-20/dB/dBkL/initial_graph.txt -l dataset/synthetic-thread-20/dB/dBkL/label_list.txt -u dataset/synthetic-thread-20/dB/dBkL/update_stream.txt -q dataset/synthetic-thread-20/dB/dBkL/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-thread-20/dB/dBkU/initial_graph.txt -l dataset/synthetic-thread-20/dB/dBkU/label_list.txt -u dataset/synthetic-thread-20/dB/dBkU/update_stream.txt -q dataset/synthetic-thread-20/dB/dBkU/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-thread-20/dB/dBkP/initial_graph.txt -l dataset/synthetic-thread-20/dB/dBkP/label_list.txt -u dataset/synthetic-thread-20/dB/dBkP/update_stream.txt -q dataset/synthetic-thread-20/dB/dBkP/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3

build/cdsbn -i dataset/synthetic-thread-20/dP/dPkL/initial_graph.txt -l dataset/synthetic-thread-20/dP/dPkL/label_list.txt -u dataset/synthetic-thread-20/dP/dPkL/update_stream.txt -q dataset/synthetic-thread-20/dP/dPkL/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-thread-20/dP/dPkU/initial_graph.txt -l dataset/synthetic-thread-20/dP/dPkU/label_list.txt -u dataset/synthetic-thread-20/dP/dPkU/update_stream.txt -q dataset/synthetic-thread-20/dP/dPkU/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-thread-20/dP/dPkP/initial_graph.txt -l dataset/synthetic-thread-20/dP/dPkP/label_list.txt -u dataset/synthetic-thread-20/dP/dPkP/update_stream.txt -q dataset/synthetic-thread-20/dP/dPkP/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
