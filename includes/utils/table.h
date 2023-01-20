#ifndef UTILS_TABLE_H_
#define UTILS_TABLE_H_

#include <unordered_map>

namespace utils
{
    template <class K, class V>
    class Table
    {
        using Map = std::unordered_map<K, V>;

        Map map;
    };

    template <class K, class V>
    class LinkedTable
    {
    };
}  // namespace utils

#endif
