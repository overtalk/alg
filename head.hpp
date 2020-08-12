#pragma once

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <set>
#include <time.h>
#include <unordered_set>
#include <vector>

////////////////////////////////////////////////
////////////////////////////////////////////////
void swap(std::vector<int> &src, int i, int j) {
  int temp = src[i];
  src[i] = src[j];
  src[j] = temp;
}

void show(const std::vector<int> &src) {
  for (const auto &i : src)
    std::cout << i << " ";
  std::cout << std::endl;
}