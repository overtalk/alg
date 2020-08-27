#pragma once

#include "head.hpp"

void build_next_table(char *target, int *next_table) {
  int target_size = strlen(target);
  next_table[0] = 0;

  for (int i = 1; i < target_size; ++i) {
    // 前缀子串
    std::string temp{target[0]};
    std::set<std::string> head_set;
    head_set.insert(temp);
    for (int j = 1; j < i; ++j) {
      temp = temp + target[j];
      head_set.insert(temp);
    }

    // 后缀子串
    temp = target[i];
    std::set<std::string> tail_set;
    tail_set.insert(temp);
    for (int j = i - 1; j > 0; --j) {
      temp = target[j] + temp;
      tail_set.insert(temp);
    }

    int max = 0;
    for (const auto &iter : head_set) {
      if (tail_set.count(iter) > 0 && iter.size() > max) {
        max = iter.size();
      }
    }

    next_table[i] = max;
  }
}

int kmp(char *src, char *target) {
  int src_size = strlen(src);
  int target_size = strlen(target);

  if ((target + src_size) == 0 || target_size > src_size) {
    return -1;
  }

  int *next = new int[target_size];
  build_next_table(target, next);

  int target_pos = 0;
  for (int i = 0; i < src_size;) {
    if (src[i] == target[target_pos]) {
      i++;
      target_pos++;
    } else {
      if (target_pos == 0) {
        i++;
      } else {
        target_pos = next[target_pos - 1];
      }
    }

    if (target_pos == target_size) {
      delete[] next;
      return i - target_pos;
    }
  }

  delete[] next;
  return -1;
}

////////////////////////////////////////
////////////////////////////////////////
void kmp_test() {
  // kmp
  std::cout << "-------------------kmp---------------------" << std::endl;
  char src[] = "xx--abababca";
  char target[] = "abababca";
  std::cout << "kmp result = " << kmp(src, target) << std::endl;
}
