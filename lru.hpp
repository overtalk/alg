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
  std::chrono::seconds ttl_;
  std::chrono::system_clock::time_point access_time_;

public:
  bool expired(std::chrono::system_clock::time_point now) {
    return (ttl_.count() > 0) &&
           (std::chrono::duration_cast<std::chrono::seconds>(now - access_time_)
                .count() > ttl_.count());
  }

  void set_assess_time(std::chrono::system_clock::time_point now) {
    access_time_ = now;
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

  // Get returns a value from the cache, and marks the entry as most recently
  // used.
  T *Get(std::string key) {
    std::lock_guard<std::mutex> lock(mutex_);

    entry<T *> *iter = table_.find(key);
    if (iter == table_.end()) {
      return nullptr;
    }

    auto now = std::chrono::system_clock::now();
    if (iter->expired(now)) {
      return nullptr;
    }

    iter->set_assess_time(now);
    MoveToFront(iter);

    return iter->value_;
  }

  // Peek returns a value from the cache without changing the LRU order.
  T *Peek(std::string key) {
    std::lock_guard<std::mutex> lock(mutex_);

    entry<T *> *iter = table_.find(key);
    if (iter == table_.end()) {
      return nullptr;
    }

    auto now = std::chrono::system_clock::now();
    if (iter->expired(now)) {
      return nullptr;
    }

    return iter->value_;
  }

  bool IsExist(std::string key) {
    std::lock_guard<std::mutex> lock(mutex_);

    entry<T *> *iter = table_.find(key);
    if (iter == table_.end()) {
      return nullptr;
    }

    auto now = std::chrono::system_clock::now();
    return iter->expired(now);
  }

  void Set(std::string key, T *value, std::chrono::seconds ttl) {
    std::lock_guard<std::mutex> lock(mutex_);

    if
      !replace_old_item(key, value, ttl) {}
  }

private:
  void add_new(std::string &key, T *value, std::chrono::seconds ttl) {}

  bool replace_old_item(std::string &key, T *value, std::chrono::seconds ttl) {
    auto now = std::chrono::system_clock::now();
    entry<T *> *iter = table_.find(key);

    // if existed, just replace its value.
    if (iter != table_.end()) {
      iter->value_ = value;
      iter->ttl_ = ttl;
      iter->access_time_ = now;
      move_to_front(iter);
      return true;
    }

    // replace expired item of spare one.
    if (table_.empty()) {
      return false;
    }

    auto iter = table_.end();
    iter--;
    if (size_ < capacity_ && !iter->expired(now)) {
      return false;
    }

    table_.erase(key);
    iter->key_ = key;
    iter->value_ = value;
    iter->ttl_ = ttl;
    iter->access_time_ = now;
    table_.insert(std::pair<std::string, entry<T *> *>(key, iter));
    move_to_front(iter);
    move_to_front(iter);
    return true;
  }

  void move_to_front(entry<T *> *iter) {
    if (iter == list_.end() || iter == list_.begin())
      return;

    list_.splice(list_.begin(), list_, iter, std::next(iter));
  }
};
