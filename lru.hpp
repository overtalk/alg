#pragma once

#include <chrono>
#include <ctime>
#include <iostream>
#include <list>
#include <mutex>
#include <unordered_map>

template <typename T> class entry {
private:
  std::string key_;
  T *value_;
  std::chrono::system_clock::time_point assess_time_;
  std::chrono::seconds ttl_;

public:
  bool expired(std::chrono::system_clock::time_point now) {
    return (ttl_.count() > 0) &&
           (std::chrono::duration_cast<std::chrono::seconds>(now - assess_time_)
                .count() > ttl_.count());
  }

  void set_assess_time(std::chrono::system_clock::time_point now) {
    assess_time_ = now;
  }
};

template <typename T> class LruCache {
private:
  int64_t size_;
  int64_t capacity_;
  std::chrono::seconds ttl_;

  std::mutex mutex_;
  std::list<entry<T *> *> list_;
  std::unordered_map<std::string, entry<T *> *> table_;

public:
  LruCache(int64_t capacity, std::chrono::seconds ttl)
      : capacity_(capacity), ttl_(ttl) {}

  void Get(std::string key) {
    std::lock_guard<std::mutex> lock(mutex_);
    entry<T *> *iter = table_.find(key);
    if (iter == table_.end()) {
      // TODO: return value
      return;
    }

    auto now = std::chrono::system_clock::now();
    if (iter->expired(now)) {
      // TODO: return value
      return;
    }

    iter->set_assess_time(now);
    MoveToFront(iter);
  }

  void MoveToFront(entry<T *> *iter) {
    if (iter != list_.begin()) {
      list_.splice(list_.begin(), list_, iter, std::next(iter));
    }
  }
};