#pragma once

#include "head.hpp"

const static int64_t min_amount = 1;

int64_t double_avg(int64_t count, int64_t amount) {
  if (count == 1)
    return amount;

  int64_t max = amount - min_amount * count;
  int64_t avg = max / count;
  int64_t avg2 = avg * 2 + min_amount;

  int64_t x = gen_random(min_amount, avg2);
  return x;
}

void red_packet(int64_t count, int64_t amount) {
  int64_t remain = amount;
  int64_t sum = 0;
  for (auto i = 0; i < count; ++i) {
    int64_t x = double_avg(count - i, remain);
    std::cout << i << " = " << x << std::endl;
    remain -= x;
    sum += x;
  }
  std::cout << "sum = " << sum << std::endl;
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
void red_packet_test() {
  std::cout << "-----------------red_packet-------------------" << std::endl;
  red_packet(10, 1000);
}
