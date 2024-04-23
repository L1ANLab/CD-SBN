# CD-SBN
The code for paper "Effective Community Detection Over Streaming Bipartite Network"

## 0. Compile

With the C++17 environment and GCC 11.4 compiler, run the commands to compile as follows:

```bash
make
```

## 1. Input Data Format
The input data consists of an initial graph, a bipartite streaming graph and a item label list.

### 1.1 initial graph 
An initial graph is formatted as a series of edges with two end-vertices in two different layers. It is a text file containing multiple lines, where each line is in the form of `<user-vertex-id> <item-vertex-id>`. For example,

```text/plain
0 0
0 1
0 2
1 0
1 1
2 1
```

### 1.2 Item Label List
A item label list is list of labels for each item vertex. It is a text file containing multiple lines, where each line is in the form of `<item-vertex-id> <label-id-list>`. Note that each label in `<label-id-list>` is separated by a comma. For example,

```text/plain
0 2,4,7,8,9
1 1,2,6,8
2 0,1,2,5
3 1,5,6,7
```


### 1.3 update stream
A update stream is formatted as a series of edges with two end-vertices in two different layers and a timestamp. It is a text file containing multiple lines, where each line is in the form of `<user-vertex-id> <item-vertex-id> <timestamp>`. For example,

```text/plain
1 2 1
2 0 2
2 2 3
```

Note that `<user-vertex-id> ` represents the id of vertex in the upper layer (user vertex) and `<item-vertex-id>` represents the id of vertex in the lower layer (item vertex), and `<item-vertex-id>` should be mentioned in item label list.


## 2. Usage
After the preparation of executable file and input data, our method can be executed by:

```bash
build/main -i <initial-graph-path> -l <item-label-list-path> -u <update-stream-path> -t <query-timestamp> -Q <query-keywords-set> -k <query-support-threshold> -r <query-maximum-radius> -s <query-score-threshold>
```

For example,

## 3. Temporal Graph

```text/plain
v1 <user-vertex-id> <bv> <ub-sup-M> <X-list> <Y-list>
v2 <item-vertex-id>
e <user-vertex-id> <item-vertex-id> <ub-sup>
v2 0 2
e 2 3
```



## Reference
Command line parser: [CLI11](https://github.com/CLIUtils/CLI11)

