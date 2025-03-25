#ifndef UTILS_TYPES
#define UTILS_TYPES

#include <chrono>
#include <climits>
#include <functional>
#include <stdlib.h>

#define NOT_EXIST UINT_MAX
#define UNMATCHED UINT_MAX
#define MAX_LEVEL UINT_MAX

const uint THREADS_NUM = 100;
// const uint MAX_LABEL = 17000; // ML
// const uint MAX_LABEL = 83000; // VU
// const uint MAX_LABEL = 160000; // CU
// const uint MAX_LABEL = 210000; // BS
const uint MAX_LABEL = 1000;
const uint SYNOPSIS_SIZE = 32;
const uint R_MAX = 3;

// Time counting
#define Get_Time() std::chrono::high_resolution_clock::now()
#define Duration(start) std::chrono::duration_cast<\
    std::chrono::microseconds>(Get_Time() - start).count()/(float)1000
#define GetUnixTimestamp(now_time) std::to_string(std::chrono::duration_cast<\
    std::chrono::microseconds>(now_time.time_since_epoch()).count())
#define Print_Time_Now(str, start) std::cout << str << Duration(start) << \
    "ms" << std::endl
#define Print_Time(str, timespan) std::cout << str << timespan << \
    "ms" << std::endl

struct InsertUnit {
    uint user_id;   // vertex id or edge source id
    uint item_id;   // edge target id
    uint timestamp; // vertex or edge label
    InsertUnit(uint user_id_, uint item_id_, uint timestamp_)
    : user_id(user_id_), item_id(item_id_), timestamp(timestamp_) {}
};

struct UserData {
    uint user_id;
    uint x_data;
    uint y_data;
    std::vector<uint> wedge_item_list;     // element: item id
    std::vector<uint> wedge_score_list;     // element: wedge score

    UserData() 
        : user_id(0), x_data(0), y_data(0), wedge_item_list(0), wedge_score_list(0) {}

    UserData(uint user_id_, uint x_data_, uint y_data_, std::vector<uint> wedge_item_list_, std::vector<uint> wedge_score_list_) 
        : user_id(user_id_), x_data(x_data_), y_data(y_data_), wedge_item_list(wedge_item_list_), wedge_score_list(wedge_score_list_) {}
};


// from boost (functional/hash):
// see http://www.boost.org/doc/libs/1_35_0/doc/html/hash/combine.html template
template <typename T>
inline void hash_combine(std::size_t &seed, const T &val) {
    seed ^= std::hash<T>()(val) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}
// auxiliary generic functions to create a hash value using a seed
template <typename T> inline void hash_val(std::size_t &seed, const T &val) {
    hash_combine(seed, val);
}
template <typename T, typename... Types>
inline void hash_val(std::size_t &seed, const T &val, const Types &... args) {
    hash_combine(seed, val);
    hash_val(seed, args...);
}

template <typename... Types>
inline std::size_t hash_val(const Types &... args) {
    std::size_t seed = 0;
    hash_val(seed, args...);
    return seed;
}

struct pair_hash {
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2> &p) const {
        return hash_val(p.first, p.second);
    }
};


#endif //UTILS_TYPES
