#ifndef UTILS_TYPES
#define UTILS_TYPES

#include <chrono>
#include <climits>
#include <functional>
#include <stdlib.h>

#define NOT_EXIST UINT_MAX
#define UNMATCHED UINT_MAX
#define MAX_LEVEL UINT_MAX

#define MAX_LABEL 100
#define SYNOPSIS_SIZE 32
#define R_MAX 3

// Time counting
#define Get_Time() std::chrono::high_resolution_clock::now()
#define Duration(start) std::chrono::duration_cast<\
    std::chrono::microseconds>(Get_Time() - start).count()/(float)1000
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
