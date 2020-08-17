#pragma once

#include "head.hpp"

const static int64_t min_amount = 1;

int64_t double_avg(int64_t count, int64_t amount) {
  if (count == 1)
    return amount;

  //计算出最大可用金额
  int64_t max = amount - min_amount * count;
  //计算出最大可用平均值
  int64_t avg = max / count;
  //二倍均值基础上再加上最小金额 防止出现金额为0
  int64_t avg2 = avg * 2 + min_amount;

  //随机红包金额序列元素，把二倍均值作为随机的最大数
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
