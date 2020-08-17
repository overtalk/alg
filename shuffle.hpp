#pragma once

#include "head.hpp"

void shuffle(std::vector<int> &src) {
  int size = src.size();
  for (int i = size - 1; i > 0; --i) {
    int pos = gen_random(0, i);
    swap(src, pos, i);
  }
}