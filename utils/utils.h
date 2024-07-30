#ifndef UTILS_UTILS
#define UTILS_UTILS

#include <algorithm>
#include <future>
#include <iostream>
#include <vector>
#include <string.h>
#include <sys/stat.h> /* For stat() */
#include "utils/types.h"

namespace ErrorControl {
    inline void assert_error(bool condition, std::string error_info)
    {
        if (condition)
        {
            std::cout << error_info << std::endl;
            exit(-1);
        }
    }
}

namespace io {
    inline size_t file_exists(const char *path) {
        struct stat st;
        return stat(path, &st) == 0;
    }
}

inline void execute_with_time_limit(std::function<void()> fun, uint time_limit, std::atomic<bool>& reach_time_limit)
{
    std::future future = std::async(std::launch::async, fun);
    std::future_status status;
    do {
        status = future.wait_for(std::chrono::seconds(time_limit));
        if (status == std::future_status::deferred)
        {
            std::cout << "Deferred" << std::endl;
            exit(-1);
        }
        else if (status == std::future_status::timeout)
        {
            reach_time_limit = true;
            std::cout << "Timeout " << time_limit << "s\n";
        }
    } while (status != std::future_status::ready);
}

namespace utils {
    inline std::vector<uint> generate_query_keywords(uint keyword_num, uint all_keyword_num)
    {
        std::vector<uint> temp;
        for (uint i = 0; i < all_keyword_num; i++) temp.push_back(i);
        srand(unsigned(time(0)));
        random_shuffle(temp.begin(), temp.end());
        temp.assign(temp.begin(), temp.begin()+keyword_num);
        temp.shrink_to_fit();
        return temp;
    }
}

#endif //UTILS_UTILS
