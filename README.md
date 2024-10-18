# CD-SBN

The code for paper "Effective Community Detection Over Streaming Bipartite Network"

## 0. Environments

Before setting up your environment, make sure you have anaconda and the g++ compiler installed

Run the following command to install the python environment:

```bash
conda env create -f environment.yaml
```

## 1.Dataset

All of used dataset is from [KONECT](http://konect.cc/). To decompress the dataset zip file, run the commands:

```bash
tar -xjf <filename> -C <target-folder>
tar -vcjf ./bd-25k.tar.bz2 -C./dataset/synthetic/dB/ out.bd-25k
```

To process the raw graph data to the following format, please run the python scripts in `/generator`.

## 2. Compile

With the C++17 environment and GCC 11.4 compiler, run the commands to compile as follows:

```bash
make
```

!!!Warning: make sure the global settings are appropriate in `/utils/types.h`. For example,

```cpp
const uint THREADS_NUM = 20;   // multi-thread optimization
const uint MAX_LABEL = 210000; // maximum label value
const uint SYNOPSIS_SIZE = 32; // the size of children list in a synopsis node
const uint R_MAX = 3;          // the maximum value of query radius
```

## 2. Input Data Format

The input data consists of an initial graph, a bipartite streaming graph and a item label list.

### 3.1 initial graph

An initial graph is formatted as a series of edges with two end-vertices in two different layers. It is a text file containing multiple lines, where each line is in the form of `<user-vertex-id> <item-vertex-id>`. For example,

```text/plain
0 0
0 1
0 2
1 0
1 1
2 1
```

### 3.2 Item Label List

A item label list is list of labels for each item vertex. It is a text file containing multiple lines, where the first line is the size of keyword domain and  each following line is in the form of `<item-vertex-id> <label-id-list>`. Note that each label in `<label-id-list>` is separated by a comma. For example,

```text/plain
20
0 2,4,7,8,9
1 1,2,6,8
2 0,1,2,5
3 1,5,6,7
```

### 3.3 update stream

A update stream is formatted as a series of edges with two end-vertices in two different layers and a timestamp. It is a text file containing multiple lines, where each line is in the form of `<user-vertex-id> <item-vertex-id> <timestamp>`. For example,

```text/plain
1 2 1
2 0 2
2 2 3
```

### 3.4 query keywords list

A update stream is a list of query keywords. It is a text file containing multiple lines, where the first line are two numbers where the former one is # of query keywords (`n`) and the latter one is the size of each query keywords (`m`). Then, the following `n` lines are query keywords list and each line consists of `m` numbers. For example,

```text/plain
5 5
1 2 3 4 5
0 2 4 6 8
1 3 5 7 9
3 6 9 10 11
4 8 12 16 20
```

Note that `<user-vertex-id>` represents the id of vertex in the upper layer (user vertex) and `<item-vertex-id>` represents the id of vertex in the lower layer (item vertex), and `<item-vertex-id>` should be mentioned in item label list.

## 4. Usage

After the preparation of executable file and input data, our method can be executed by:

```bash
build/cdsbn <-c> -i <initial-graph-path> -l <item-label-list-path> -u <update-stream-path> -t <query-timestamp> -w <sliding-window-size> -q <query-keywords-path> -k <query-support-threshold> -r <query-maximum-radius> -s <query-score-threshold>
```

For example,

```bash
build/cdsbn -i dataset/realworld/BS/initial_graph.txt -l dataset/realworld/BS/label_list.txt -u dataset/realworld/BS/update_stream.txt -q dataset/realworld/BS/query_keywords_list-5.txt -t 599996400 -w 100 -k 4 -r 2 -s 2

build/cdsbn -i dataset/realworld/BS/initial_graph.txt -l dataset/realworld/BS/label_list.txt -u dataset/realworld/BS/update_stream.txt -q dataset/realworld/BS/query_keywords_list-10.txt -t 599996400 -w 100 -k 4 -r 2 -s 2

build/cdsbn -i dataset/realworld/BS/initial_graph.txt -l dataset/realworld/BS/label_list.txt -u dataset/realworld/BS/update_stream.txt -q dataset/realworld/BS/query_keywords_list-20.txt -t 599996400 -w 100 -k 4 -r 2 -s 2
```

## 5. Performance test

```bash
gprof -b -p -q build/cdsbn gmon.out > report.txt

valgrind --log-file=valReport --leak-check=full --show-reachable=yes --leak-resolution=low build/cdsbn -i dataset/realworld/BS/initial_graph.txt -l dataset/realworld/BS/label_list.txt -u dataset/realworld/BS/update_stream.txt -q dataset/realworld/BS/query_keywords_list-1.txt -t 0 -w 100 -k 4 -r 2 -s 2

```

## Reference

Command line parser: [CLI11](https://github.com/CLIUtils/CLI11)
Dataset: [KONECT](http://konect.cc/)
