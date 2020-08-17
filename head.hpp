#pragma once

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <set>
#include <unordered_set>
#include <vector>

int gen_random(int MIN, int MAX) {
  srand((unsigned)time(NULL));
  return (rand() % (MAX - MIN)) + MIN;
}
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