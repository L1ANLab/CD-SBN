cd /home/yons/projects/CD-SBN/

build/cdsbn -i dataset/synthetic/dB/dBkL/initial_graph.txt -l dataset/synthetic/dB/dBkL/label_list.txt -u dataset/synthetic/dB/dBkL/update_stream.txt -q dataset/synthetic/dB/dBkL/query_keywords_list-5.txt -t 0 -w 100 -k 4 -r 2 -s 2
build/cdsbn -i dataset/synthetic/dP/dPkL/initial_graph.txt -l dataset/synthetic/dP/dPkL/label_list.txt -u dataset/synthetic/dP/dPkL/update_stream.txt -q dataset/synthetic/dP/dPkL/query_keywords_list-5.txt -t 0 -w 100 -k 4 -r 2 -s 2

build/cdsbn -i dataset/synthetic/dB/dBkP/initial_graph.txt -l dataset/synthetic/dB/dBkP/label_list.txt -u dataset/synthetic/dB/dBkP/update_stream.txt -q dataset/synthetic/dB/dBkP/query_keywords_list-5.txt -t 0 -w 100 -k 4 -r 2 -s 2
build/cdsbn -i dataset/synthetic/dP/dPkP/initial_graph.txt -l dataset/synthetic/dP/dPkP/label_list.txt -u dataset/synthetic/dP/dPkP/update_stream.txt -q dataset/synthetic/dP/dPkP/query_keywords_list-5.txt -t 0 -w 100 -k 4 -r 2 -s 2

build/cdsbn -i dataset/synthetic/dB/dBkU/initial_graph.txt -l dataset/synthetic/dB/dBkU/label_list.txt -u dataset/synthetic/dB/dBkU/update_stream.txt -q dataset/synthetic/dB/dBkU/query_keywords_list-5.txt -t 0 -w 100 -k 4 -r 2 -s 2
build/cdsbn -i dataset/synthetic/dP/dPkU/initial_graph.txt -l dataset/synthetic/dP/dPkU/label_list.txt -u dataset/synthetic/dP/dPkU/update_stream.txt -q dataset/synthetic/dP/dPkU/query_keywords_list-5.txt -t 0 -w 100 -k 4 -r 2 -s 2
