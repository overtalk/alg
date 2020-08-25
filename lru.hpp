#pragma once

#include <chrono>
#include <ctime>
#include <iostream>
#include <list>
#include <mutex>
#include <unordered_map>

template <typename key_t, typename value_t> class entry {
public:
  key_t key_;
  value_t value_;
  std::chrono::seconds ttl_;
  std::chrono::system_clock::time_point access_time_;

  entry(key_t key, value_t value, std::chrono::seconds ttl,
        std::chrono::system_clock::time_point access_time)
      : key_(key), value_(value), ttl_(ttl), access_time_(access_time) {}

  entry(key_t key, value_t value,
        std::chrono::system_clock::time_point access_time)
      : key_(key), value_(value), ttl_(ttl_), access_time_(access_time) {}

  entry(key_t key, value_t value, std::chrono::seconds ttl)
      : key_(key), value_(value), ttl_(ttl),
        access_time_(std::chrono::system_clock::now()) {}

  bool expired(std::chrono::system_clock::time_point now) {
    return (ttl_.count() > 0) &&
           (std::chrono::duration_cast<std::chrono::seconds>(now - access_time_)
                .count() > ttl_.count());
  }

  // void set_access_time(std::chrono::system_clock::time_point now) {
  //  access_time_ = now;
  //}
  // void set_ttl(std::chrono::seconds ttl) { ttl_ = ttl; }
  // void set_key(key_t key) { key_ = key; }
  // void set_value(value_t value) { value_ = value; }

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
  using kv_pair_t = std::pair<key_t, entry_t_ptr>;
  using list_iterator_t = typename std::list<kv_pair_t>::iterator;

private:
  int64_t size_;
  int64_t capacity_;
  std::chrono::seconds ttl_;

  std::mutex mutex_;
  std::list<kv_pair_t> items_list_; // list store the real data
  std::unordered_map<key_t, list_iterator_t>
      items_map_; // map store the key-iter pair

public:
  LruCache(int64_t capacity, std::chrono::seconds ttl = std::chrono::seconds(0))
      : capacity_(capacity), ttl_(ttl) {}

  // Get returns a value from the cache, and marks the entry as most recently
  // used.
  value_t &Get(key_t key) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto iter = items_map_.find(key);
    if (iter == items_map_.end()) {
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

    auto iter = items_map_.find(key);
    if (iter == items_map_.end()) {
      return nullptr;
    }

    auto now = std::chrono::system_clock::now();
    if (iter->expired(now)) {
      return nullptr;
    }

    return iter->value_;
  }

  // IsExisted check whether a value is existed in the cache and not expired.
  bool IsExist(key_t key) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto iter = items_map_.find(key);
    if (iter == items_map_.end()) {
      return nullptr;
    }

    auto now = std::chrono::system_clock::now();
    return iter->expired(now);
  }

  // SetWithTTL sets a value in the cache with a TTL.
  void SetWithTTL(key_t key, value_t value, std::chrono::seconds ttl) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!replace_old_item(key, value, ttl)) {
      add_new(key, std::move(value), ttl);
    }
  }

  // Set sets a value in the cache with a TTL.
  void Set(key_t key, value_t value) { SetWithTTL(key, value, ttl_); }

  // SetIfAbsent will set the value in the cache if not present.
  // If the value exists in the cache, we don't set it.
  void SetIfAbsent(key_t key, value_t value) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto iter = items_map_.find(key);
    if (iter != items_map_.end()) {
      auto now = std::chrono::system_clock::now();
      if (!iter->second->second->expired(now)) {
        iter->second->second->ttl_ = ttl;
        iter->second->second->access_time_ = now;
        move_to_front(iter);
      }
    }

    if (!replace_old_item(key, value, ttl)) {
      add_new(key, std::move(value), ttl);
    }
  }

  // SetExpired will set an entry expired from the cache and returns if the
  // entry existed.
  bool SetExpired(key_t key) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto iter = items_map_.find(key);
    if (iter != items_map_.end()) {
      iter->second->second->ttl_ = 1;
      return true;
    }

    return false;
  }

  // Delete removes an entry from the cache, and returns if the entry existed.
  bool Delete(key_t key) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto iter = items_map_.find(key);
    if (iter != items_map_.end()) {
      items_list_.erase(iter);
      size_--;
      items_map_.erase(key);
      return true;
    }
    return false;
  }
  // Clear will clear the entire cache.
  void Clear() {
    std::lock_guard<std::mutex> lock(mutex_);

    size_ = 0;
    items_list_.clear();
    items_map_.clear();
  }

  // SetCapacity will set the capacity of the cache. If the capacity is
  // smaller, and the current cache size exceed that capacity, the cache
  // will be shrank.
  void SetCapacity(size_t capacity) {
    std::lock_guard<std::mutex> lock(mutex_);
    capacity_ = capacity;
    check_capacity();
  }

private:
  void check_capacity() {
    while (size_ > capacity_) {
      auto iter = items_list_.end();
      iter--;
      items_map_.erase(iter->second->key_);
      items_list_.erase(iter);
      size_--;
    }
  }

  void add_new(key_t key, value_t value, std::chrono::seconds ttl) {
    items_list_.push_front(
        std::make_pair(key, &entry_t(key, std::move(value), ttl)));
    size_++;
    items_map_.insert(std::make_pair(key, items_list_.begin()));
    check_capacity();
  }

  bool replace_old_item(key_t &key, value_t &value, std::chrono::seconds ttl) {
    auto now = std::chrono::system_clock::now();
    auto iter = items_map_.find(key);

    // if existed, just replace its value.
    if (iter != items_map_.end()) {
      iter->second->second->value_ = value;
      iter->second->second->ttl_ = ttl;
      iter->second->second->access_time_ = now;
      move_to_front(iter->second);
      return true;
    }

    // if the list is empty
    if (items_list_.empty()) {
      return false;
    }

    // replace expired item of spare one.
    iter = items_map_.end();
    iter--;
    if (size_ < capacity_ && !iter->second->second->expired(now)) {
      return false;
    }

    items_map_.erase(key);
    iter->second->second->key_ = key;
    iter->second->second->value_ = value;
    iter->second->second->ttl_ = ttl;
    iter->second->second->access_time_ = now;
    items_map_.insert(std::move(std::make_pair(key, iter->second)));
    move_to_front(iter->second);
    return true;
  }

  // move_to_front move the iter to front
  void move_to_front(list_iterator_t &iter) {
    if (iter == items_list_.begin() || iter == items_list_.end())
      return;

    items_list_.splice(items_list_.begin(), items_list_, iter);
  }
};

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
void lru_test() {
  LruCache<int, int> cache(100, std::chrono::seconds(60));
  cache.SetWithTTL(1, 2, std::chrono::seconds(60));
}
