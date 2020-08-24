#pragma once

#include <chrono>
#include <ctime>
#include <iostream>
#include <list>
#include <mutex>
#include <unordered_map>

template <typename key_t, typename value_t> class entry {
private:
  key_t key_;
  value_t value_;
  std::chrono::seconds ttl_;
  std::chrono::system_clock::time_point access_time_;

public:
  entry(key_t key, value_t value, std::chrono::seconds ttl,
        std::chrono::system_clock::time_point access_time)
      : key_(key), value_(value), ttl_(ttl), access_time_(access_time) {}

  bool expired(std::chrono::system_clock::time_point now) {
    return (ttl_.count() > 0) &&
           (std::chrono::duration_cast<std::chrono::seconds>(now - access_time_)
                .count() > ttl_.count());
  }

  void set_access_time(std::chrono::system_clock::time_point now) {
    access_time_ = now;
  }
  void set_ttl(std::chrono::seconds ttl) { ttl_ = ttl; }
  void set_key(key_t key) { key_ = key; }
  void set_value(value_t value) { value_ = value; }

  ~entry() {
    // TODO: del from heap?
  }
};

/// <summary>
/// LruCache defines a lru cache
/// </summary>
/// <typeparam name="key_t"></typeparam>
/// <typeparam name="value_t"></typeparam>
template <typename key_t, typename value_t> class LruCache {
public:
  using entry_t = entry<key_t, value_t>;
  using entry_t_ptr = entry<key_t, value_t> *;

private:
  int64_t size_;
  int64_t capacity_;
  std::chrono::seconds ttl_;

  std::mutex mutex_;
  std::list<entry_t_ptr> list_;
  std::unordered_map<key_t, entry_t_ptr> table_;

public:
  LruCache(int64_t capacity, std::chrono::seconds ttl = std::chrono::seconds(0))
      : capacity_(capacity), ttl_(ttl) {}

  // Get returns a value from the cache, and marks the entry as most recently
  // used.
  value_t &Get(key_t key) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto iter = table_.find(key);
    if (iter == table_.end()) {
      return nullptr;
    }

    auto now = std::chrono::system_clock::now();
    if (iter->expired(now)) {
      return nullptr;
    }

    iter->set_assess_time(now);
    move_to_front(iter);

    return iter->value_;
  }

  // Peek returns a value from the cache without changing the LRU order.
  value_t &Peek(key_t key) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto iter = table_.find(key);
    if (iter == table_.end()) {
      return nullptr;
    }

    auto now = std::chrono::system_clock::now();
    if (iter->expired(now)) {
      return nullptr;
    }

    return iter->value_;
  }

  bool IsExist(key_t key) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto iter = table_.find(key);
    if (iter == table_.end()) {
      return nullptr;
    }

    auto now = std::chrono::system_clock::now();
    return iter->expired(now);
  }

  void Set(key_t key, value_t &&value, std::chrono::seconds ttl) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!replace_old_item(key, value, ttl)) {
    }
  }

private:
  void add_new(std::string &&key, value_t &&value, std::chrono::seconds ttl) {
    auto now = std::chrono::system_clock::now();
    entry_t e(key, std::move(value), now, ttl);
    std::pair<key_t, entry_t_ptr> pair(key, std::move(&e));

    list_.insert(pair);
    table_.insert(key, pair)
  }

  bool replace_old_item(key_t &key, value_t &value, std::chrono::seconds ttl) {
    auto now = std::chrono::system_clock::now();
    auto iter = table_.find(key);

    // if existed, just replace its value.
    if (iter != table_.end()) {
      iter->second->set_value(value);
      iter->second->set_ttl(ttl);
      iter->second->set_access_time(now);
      move_to_front(iter->second);
      return true;
    }

    // replace expired item of spare one.
    if (table_.empty()) {
      return false;
    }

    iter = table_.end();
    iter--;
    if (size_ < capacity_ && !iter->second->expired(now)) {
      return false;
    }

    table_.erase(key);
    iter->second->set_key(key);
    iter->second->set_value(value);
    iter->second->set_ttl(ttl);
    iter->second->set_access_time(now);
    table_.insert(std::move(std::make_pair(key, iter->second)));
    move_to_front(iter->second);
    return true;
  }

  // move_to_front move the iter to front
  void move_to_front(entry_t_ptr iter) {
    if (iter == list_.begin() || iter == list_.end())
      return;

    list_.splice(list_.begin(), list_, iter, std::next(iter));
  }
};

void lru_test() {
  LruCache<int, int> cache(100, std::chrono::seconds(60));
  cache.Set(1, 2, std::chrono::seconds(60));
}
