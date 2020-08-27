#pragma once

#include "head.hpp"

int bin_search_t(const std::vector<int>& src, int target, int l, int r)
{
    if (l > r)
        return -1;

    int mid = (l + r) / 2;
    if (src[mid] == target)
    {
        return mid;
    }
    else if (src[mid] < target)
    {
        return bin_search_t(src, target, mid + 1, r);
    }

    return bin_search_t(src, target, l, mid - 1);
}

int bin_search(const std::vector<int>& src, int target)
{
    return bin_search_t(src, target, 0, src.size() - 1);
}

////////////////////////////////////////////////
////////////////////////////////////////////////
void search_test()
{
    std::vector<int> search_src;
    for (int i = 0; i < 15; ++i)
    {
        search_src.push_back(i);
    }

    // search about
    std::cout << "----------------bin_search------------------" << std::endl;
    std::cout << "bin search result = " << bin_search(search_src, 5) << std::endl;
}
