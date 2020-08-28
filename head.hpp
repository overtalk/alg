#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <list>
#include <set>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <queue>
#include <atomic>
#include <chrono>
#include <chrono>
#include <ctime>
#include <iostream>
#include <list>
#include <mutex>
#include <unordered_map>
#include <thread>

////////////////////////////////////////////////
////////////////////////////////////////////////
int gen_random(int MIN, int MAX)
{
    srand((unsigned)time(NULL));
    return (rand() % (MAX - MIN)) + MIN;
}

////////////////////////////////////////////////
////////////////////////////////////////////////
void swap(std::vector<int>& src, int i, int j)
{
    int temp = src[i];
    src[i] = src[j];
    src[j] = temp;
}

void show(const std::vector<int>& src)
{
    for (const auto& i : src)
        std::cout << i << " ";
    std::cout << std::endl;
}
