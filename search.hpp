#pragma once

#include "head.hpp"

int bin_search_t(const std::vector<int> &src, int target, int l, int r) {
  if (l > r) {
    return -1;
  }

  int mid = (l + r) / 2;
  if (src[mid] == target) {
    return mid;
  } else if (src[mid] < target) {
    return bin_search_t(src, target, mid + 1, r);
  }

  return bin_search_t(src, target, l, mid - 1);
}

int bin_search(const std::vector<int> &src, int target) {
  return bin_search_t(src, target, 0, src.size() - 1);
}