#pragma once

#include "head.hpp"

////////////////////////////////////////////////
////////////////////////////////////////////////
void bubble(std::vector<int> &src) {
  int size = src.size();
  for (int i = size - 1; i > 0; --i) {
    for (int j = 0; j < i; ++j) {
      if (src[j] > src[j + 1]) {
        swap(src, j, j + 1);
      }
    }
  }
}

////////////////////////////////////////////////
////////////////////////////////////////////////
void select_sort(std::vector<int> &src) {
  int size = src.size();
  for (int i = 0; i < size; ++i) {
    int min = i;

    for (int j = i + 1; j < size; ++j) {
      if (src[j] < src[min]) {
        min = j;
      }
    }

    if (min != i) {
      swap(src, min, i);
    }
  }
}

////////////////////////////////////////////////
////////////////////////////////////////////////
void quick_sort_i(std::vector<int> &src, int l, int r) {
  if (l > r) {
    return;
  }

  int left = l;
  int right = r;
  int base_value = src[l];

  while (left < right) {
    while (left < right && src[right] >= base_value) {
      right--;
    }

    while (left < right && src[left] <= base_value) {
      left++;
    }

    if (left < right) {
      swap(src, left, right);
    }
  }

  src[l] = src[left];
  src[left] = base_value;

  quick_sort_i(src, l, left - 1);
  quick_sort_i(src, left + 1, r);
}

void quick_sort(std::vector<int> &src) { quick_sort_i(src, 0, src.size() - 1); }

////////////////////////////////////////////////
////////////////////////////////////////////////
void sink(std::vector<int> &src, int k, int N) {
  while (true) {
    int i = k * 2;

    if (i > N) {
      break;
    }

    if (i < N && src[i + 1] > src[i]) {
      i++;
    }

    if (src[i] > src[k]) {
      swap(src, k, i);
    }

    k = i;
  }
}

void heap_sort(std::vector<int> &src) {
  int N = src.size();

  if (N == 0) {
    return;
  }

  src.insert(src.begin(), 0);

  for (int k = N / 2; k > 0; --k) {
    sink(src, k, N);
  }

  while (N > 1) {
    swap(src, 1, N);
    N--;
    sink(src, 1, N);
  }

  src.erase(src.begin());
}