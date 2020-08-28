#pragma once

#include "head.hpp"

template<typename key_t, typename value_t>
class entry
{
public:
    key_t key_;
    value_t value_;
    std::chrono::seconds ttl_;
    std::chrono::system_clock::time_point access_time_;

    entry(key_t key, value_t value, std::chrono::seconds ttl, std::chrono::system_clock::time_point access_time)
        : key_(key)
        , value_(value)
        , ttl_(ttl)
        , access_time_(access_time)
    {
    }

    entry(key_t key, value_t value, std::chrono::system_clock::time_point access_time)
        : key_(key)
        , value_(value)
        , ttl_(ttl_)
        , access_time_(access_time)
    {
    }

    entry(key_t key, value_t value, std::chrono::seconds ttl)
        : key_(key)
        , value_(value)
        , ttl_(ttl)
        , access_time_(std::chrono::system_clock::now())
    {
    }

    bool expired(std::chrono::system_clock::time_point now = std::chrono::system_clock::now())
    {
        return (ttl_.count() >= 0) &&
               (std::chrono::duration_cast<std::chrono::seconds>(now - access_time_).count() > ttl_.count());
    }
};

/// <summary>
/// LruCache defines a LRU cache
/// </summary>
/// <typeparam name="key_t"></typeparam>
/// <typeparam name="value_t"></typeparam>
template<typename key_t, typename value_t>
class LruCache
{
public:
    using entry_t = entry<key_t, value_t>;
    using list_iterator_t = typename std::list<entry_t>::iterator;

private:
    int64_t size_{0};
    int64_t capacity_;
    std::mutex mutex_;
    std::chrono::seconds ttl_;

    std::list<entry_t> entry_list_;                            // list store the real data
    std::unordered_map<key_t, list_iterator_t> list_iter_map_; // map store the key-iter pair

public:
    LruCache(int64_t capacity, std::chrono::seconds ttl = std::chrono::seconds(-1))
        : capacity_(capacity)
        , ttl_(ttl)
    {
    }

    // Get returns a value from the cache, and marks the entry as most recently
    // used.
    bool Get(key_t key, value_t& value)
    {
        std::lock_guard<std::mutex> lock(mutex_);

        auto map_iter = list_iter_map_.find(key);
        if (map_iter == list_iter_map_.end())
        {
            return false;
        }

        auto now = std::chrono::system_clock::now();
        if (map_iter->second->expired(now))
        {
            return false;
        }

        map_iter->second->access_time_ = now;
        move_to_front(map_iter->second);

        value = map_iter->second->value_;
        return true;
    }

    // Peek returns a value from the cache without changing the LRU order.
    bool Peek(key_t key, value_t& value)
    {
        std::lock_guard<std::mutex> lock(mutex_);

        auto map_iter = list_iter_map_.find(key);
        if (map_iter == list_iter_map_.end())
        {
            return false;
        }

        auto now = std::chrono::system_clock::now();
        if (map_iter->second->expired(now))
        {
            return false;
        }

        value = map_iter->second->value_;
        return true;
    }

    // IsExisted check whether a value is existed in the cache and not expired.
    bool IsExist(key_t key)
    {
        std::lock_guard<std::mutex> lock(mutex_);

        auto map_iter = list_iter_map_.find(key);
        if (map_iter == list_iter_map_.end())
        {
            return false;
        }

        return map_iter->second->expired();
    }

    // SetWithTTL sets a value in the cache with a TTL.
    void SetWithTTL(key_t key, value_t value, std::chrono::seconds ttl)
    {
        std::lock_guard<std::mutex> lock(mutex_);

        set_value(std::move(key), std::move(value), ttl);
    }

    // Set sets a value in the cache with a TTL.
    void Set(key_t key, value_t value)
    {
        SetWithTTL(std::move(key), std::move(value), ttl_);
    }

    // SetIfAbsent will set the value in the cache if not present.
    // If the value exists in the cache, we don't set it.
    void SetIfAbsent(key_t key, value_t value)
    {
        std::lock_guard<std::mutex> lock(mutex_);

        auto map_iter = list_iter_map_.find(key);
        if (map_iter != list_iter_map_.end())
        {
            auto now = std::chrono::system_clock::now();
            if (!map_iter->second->expired(now))
            {
                map_iter->second->ttl_ = ttl_;
                map_iter->second->access_time_ = now;
                move_to_front(map_iter->second);
            }
        }

        set_value(std::move(key), std::move(value), ttl_);
    }

    // SetExpired will set an entry expired from the cache and returns if the
    // entry existed.
    bool SetExpired(key_t key)
    {
        std::lock_guard<std::mutex> lock(mutex_);

        auto map_iter = list_iter_map_.find(key);
        if (map_iter != list_iter_map_.end())
        {
            map_iter->second->ttl_ = std::chrono::seconds(0);
            return true;
        }

        return false;
    }

    // Delete removes an entry from the cache, and returns if the entry existed.
    bool Delete(key_t key)
    {
        std::lock_guard<std::mutex> lock(mutex_);

        auto iter = list_iter_map_.find(key);
        if (iter != list_iter_map_.end())
        {
            entry_list_.erase(iter);
            size_--;
            list_iter_map_.erase(key);
            return true;
        }
        return false;
    }

    // Clear will clear the entire cache.
    void Clear()
    {
        std::lock_guard<std::mutex> lock(mutex_);

        size_ = 0;
        entry_list_.clear();
        list_iter_map_.clear();
    }

    // Length returns how many elements are in the cache
    int64_t Length()
    {
        std::lock_guard<std::mutex> lock(mutex_);

        return entry_list_.size();
    }

    // Size returns the sum of the objects' Size() method.
    int64_t Size()
    {
        std::lock_guard<std::mutex> lock(mutex_);

        return size_;
    }

    // Capacity returns the cache maximum capacity.
    int64_t Capacity()
    {
        std::lock_guard<std::mutex> lock(mutex_);

        return capacity_;
    }

    // FreeSize returns the cache's free capacity.
    int64_t FreeSize()
    {
        std::lock_guard<std::mutex> lock(mutex_);

        return capacity_ - size_;
    }

    // SetCapacity will set the capacity of the cache. If the capacity is
    // smaller, and the current cache size exceed that capacity, the cache
    // will be shrank.
    void SetCapacity(int64_t capacity)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        capacity_ = capacity;
        check_capacity();
    }

private:
    void check_capacity()
    {
        while (size_ > capacity_)
        {
            std::cout << "size = " << size_ << std::endl;
            auto iter = entry_list_.end();
            iter--;
            list_iter_map_.erase(iter->key_);
            entry_list_.erase(iter);
            size_--;
        }
    }

    // add a new value
    void set_value(key_t&& key, value_t&& value, std::chrono::seconds ttl)
    {
        auto now = std::chrono::system_clock::now();

        // replace old item if exist
        auto map_iter = list_iter_map_.find(key);

        // if existed, just replace its value.
        if (map_iter != list_iter_map_.end())
        {
            map_iter->second->value_ = value;
            map_iter->second->ttl_ = ttl;
            map_iter->second->access_time_ = now;
            move_to_front(map_iter->second);
            return;
        }

        // if the list is empty
        if (!entry_list_.empty())
        {
            // replace expired item of spare one.
            auto list_iter = entry_list_.end();
            list_iter--;
            if (list_iter->expired(now))
            {
                list_iter_map_.erase(key);
                list_iter->key_ = key;
                list_iter->value_ = value;
                list_iter->ttl_ = ttl;
                list_iter->access_time_ = now;
                list_iter_map_.insert(std::move(std::make_pair(key, list_iter)));
                move_to_front(list_iter);
                return;
            }
        }

        entry_list_.push_front(entry_t(key, std::move(value), ttl));
        size_++;
        list_iter_map_.insert(std::make_pair(key, entry_list_.begin()));
        check_capacity();
    }

    // move_to_front move the iter to front
    void move_to_front(list_iterator_t& iter)
    {
        if (iter == entry_list_.begin() || iter == entry_list_.end())
            return;

        entry_list_.splice(entry_list_.begin(), entry_list_, iter);
    }
};

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
void lru_test()
{
    int res = 0;
    LruCache<int, int> cache(100, std::chrono::seconds(60));
    cache.Peek(10, res);
    std::cout << "-------------------LRU cache---------------------" << std::endl;
    cache.SetWithTTL(1, 20, std::chrono::seconds(2));
    std::cout << "item 1 is exist : " << cache.IsExist(1) << std::endl;
    std::cout << cache.Get(1, res) << " - " << res << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(3 * 1000));
    std::cout << cache.Get(1, res) << " - " << res << std::endl;

    cache.SetIfAbsent(2, 200);
    std::cout << cache.Get(2, res) << " - " << res << std::endl;
    cache.SetExpired(2);
    std::this_thread::sleep_for(std::chrono::milliseconds(1 * 1000));
    std::cout << cache.Get(2, res) << " - " << res << std::endl;
}
