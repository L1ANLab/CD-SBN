cd /home/msc/zn/CD-SBN/

build/cdsbn -i dataset/synthetic-gamma/dB/dBkL/initial_graph.txt -l dataset/synthetic-gamma/dB/dBkL/label_list.txt -u dataset/synthetic-gamma/dB/dBkL/update_stream.txt -q dataset/synthetic-gamma/dB/dBkL/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-gamma/dP/dPkL/initial_graph.txt -l dataset/synthetic-gamma/dP/dPkL/label_list.txt -u dataset/synthetic-gamma/dP/dPkL/update_stream.txt -q dataset/synthetic-gamma/dP/dPkL/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3

build/cdsbn -i dataset/synthetic-gamma/dB/dBkP/initial_graph.txt -l dataset/synthetic-gamma/dB/dBkP/label_list.txt -u dataset/synthetic-gamma/dB/dBkP/update_stream.txt -q dataset/synthetic-gamma/dB/dBkP/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-gamma/dP/dPkP/initial_graph.txt -l dataset/synthetic-gamma/dP/dPkP/label_list.txt -u dataset/synthetic-gamma/dP/dPkP/update_stream.txt -q dataset/synthetic-gamma/dP/dPkP/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3

build/cdsbn -i dataset/synthetic-gamma/dB/dBkU/initial_graph.txt -l dataset/synthetic-gamma/dB/dBkU/label_list.txt -u dataset/synthetic-gamma/dB/dBkU/update_stream.txt -q dataset/synthetic-gamma/dB/dBkU/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
build/cdsbn -i dataset/synthetic-gamma/dP/dPkU/initial_graph.txt -l dataset/synthetic-gamma/dP/dPkU/label_list.txt -u dataset/synthetic-gamma/dP/dPkU/update_stream.txt -q dataset/synthetic-gamma/dP/dPkU/query_keywords_list-5.txt -t 0 -w 500 -k 4 -r 2 -s 3
