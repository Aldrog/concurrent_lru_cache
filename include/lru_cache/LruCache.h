#ifndef LRU_CACHE_LRUCACHE_H
#define LRU_CACHE_LRUCACHE_H

#include <atomic>
#include <forward_list>
#include <memory>
#include <mutex>
#include <unordered_map>

#include <iostream>

template <typename Key, typename Value> class LruCache {
  using HandleBase = std::shared_ptr<Value>;

public:
  explicit LruCache(size_t max_unused = 10) : max_unused{max_unused} {}

  class Handle : public HandleBase {
  public:
    ~Handle() {
      if (this->use_count() == 2) // Current handle and cached value
        parent.unuse(key);
    }

  private:
    friend LruCache<Key, Value>;
    Handle(LruCache &parent, const Key &key, const HandleBase &value)
        : std::shared_ptr<Value>(value), parent{parent}, key{key} {
      if (this->use_count() == 2) // Current handle and cached value
        parent.use(key);
    }

    LruCache<Key, Value> &parent;
    Key key;
  };

  template <typename... Args> void emplace(const Key &key, Args &&... args) {
    map.emplace(key, std::make_shared<Value>(std::forward<Args>(args)...));
    unuse(key);
  }

  Handle at(const Key &key) { return Handle{*this, key, map.at(key)}; }

  bool contains(const Key &key) const { return map.count(key); }

private:
  friend Handle;
  void use(const Key &key) {
    {
      std::scoped_lock lock{list_mutex};
      for (auto it = unused.begin(), prev = unused.before_begin();
           it != unused.end(); ++it, ++prev)
        if (*it == key) {
          if (++it == unused.end())
            unused_back = prev;
          unused.erase_after(prev);
          break;
        }
    }
    unused_size--;
    std::cout << "Using " << key << ". " << unused_size
              << " unused elements.\n";
  }

  void unuse(const Key &key) {
    {
      std::scoped_lock lock{list_mutex};
      unused_back = unused.insert_after(unused_back, key);
    }
    std::cout << "Unusing " << key << ". " << unused_size + 1
              << " unused elements.\n";
    if (++unused_size > max_unused)
      cleanup();
  }

  void cleanup() {
    auto key = [this]() {
      std::scoped_lock lock{list_mutex};
      Key lru = unused.front();
      unused.pop_front();
      return lru;
    }();
    std::cout << "Erasing " << key << ". " << unused_size - 1
              << " unused elements.\n";
    unused_size--;
    map.erase(key);
  }

  std::unordered_map<Key, HandleBase> map;
  std::forward_list<Key> unused;
  typename std::forward_list<Key>::const_iterator unused_back =
      unused.before_begin();
  std::atomic<size_t> unused_size{0};
  std::mutex list_mutex;
  const size_t max_unused = 10;
};

#endif // LRU_CACHE_LRUCACHE_H
