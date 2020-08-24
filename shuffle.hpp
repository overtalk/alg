#pragma once

#include "head.hpp"

void shuffle(std::vector<int> &src) {
  int size = src.size();
  for (int i = size - 1; i > 0; --i) {
    int pos = gen_random(0, i);
    swap(src, pos, i);
  }
}

////////////////////////////////////////////////
////////////////////////////////////////////////
void shuffle_test() {
  std::vector<int> shuffle_src;
  for (int i = 15; i > 0; --i) {
    shuffle_src.push_back(i);
  }

  // shuffle about
  shuffle(shuffle_src);
  std::cout << "-------------------shuffle---------------------" << std::endl;
  show(shuffle_src);
}
