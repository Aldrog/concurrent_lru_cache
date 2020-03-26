#ifndef LRU_CACHE_LRUCACHE_H
#define LRU_CACHE_LRUCACHE_H

#include "Log.h"

#include <atomic>
#include <forward_list>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>

namespace lru_cache {

namespace detail {

template <typename Key, typename Value> struct Defaults {
  static constexpr auto initialize = [](const Key &) { return Value{}; };
};

} // namespace detail

template <typename Key, typename Value,
          typename InitFunctor =
              decltype(detail::Defaults<Key, Value>::initialize)>
class LruCache {
  using HandleBase = std::shared_ptr<Value>;

public:
  explicit LruCache(size_t max_unused,
                    const InitFunctor &init_function =
                        detail::Defaults<Key, Value>::initialize)
      : max_unused{max_unused}, init_function{init_function} {}

  LruCache(size_t max_unused, InitFunctor &&init_function)
      : max_unused{max_unused}, init_function{
                                    std::forward<InitFunctor>(init_function)} {}

  class Handle : public HandleBase {
  public:
    ~Handle() {
      if (this->use_count() == 2) // Current handle and cached value
        parent.unuse(key);
    }

  private:
    friend LruCache;
    Handle(LruCache &parent, const Key &key, const HandleBase &value)
        : std::shared_ptr<Value>(value), parent{parent}, key{key} {
      if (this->use_count() == 2) // Current handle and cached value
        parent.use(key);
    }

    LruCache &parent;
    Key key;
  };

  template <typename... Args>
  std::pair<Handle, bool> emplace(const Key &key, Args &&... args) {
    HandleBase result;
    bool inserted;
    {
      std::scoped_lock lock{map_mutex};
      auto [it, ins] = map.emplace(
          key, std::make_shared<Value>(std::forward<Args>(args)...));
      result = it->second;
      inserted = ins;
    }
    // No need to explicitly unuse key as it's implicitly used by result
    return {Handle{*this, key, result}, inserted};
  }

  Handle at(const Key &key) {
    std::shared_lock lock{map_mutex};
    return Handle{*this, key, map.at(key)};
  }

  bool contains(const Key &key) const {
    std::shared_lock lock{map_mutex};
    return map.count(key);
  }

  Handle operator[](const Key &key) {
    HandleBase result;
    {
      std::scoped_lock lock{map_mutex};
      if (!map.count(key)) {
        result = std::make_shared<Value>(init_function(key));
        map.emplace(key, result);
      }
    }
    if (result) {
      return Handle{*this, key, result};
    }
    std::shared_lock lock{map_mutex};
    return Handle{*this, key, map.at(key)};
  }

private:
  friend Handle;
  void use(const Key &key) {
    {
      std::scoped_lock lock{list_mutex};
      for (auto it = unused.begin(), prev = unused.before_begin();
           it != unused.end(); ++it, ++prev) {
        if (*it == key) {
          if (++it == unused.end())
            unused_back = prev;
          unused.erase_after(prev);
          break;
        }
      }
    }
    unused_size--;
    log << "Using " << key << ". " << unused_size << " unused elements.\n";
  }

  void unuse(const Key &key) {
    {
      std::scoped_lock lock{list_mutex};
      unused_back = unused.insert_after(unused_back, key);
    }
    log << "Unusing " << key << ". " << unused_size + 1
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
    log << "Erasing " << key << ". " << unused_size - 1
        << " unused elements.\n";
    unused_size--;
    std::scoped_lock lock{map_mutex};
    map.erase(key);
  }

  std::unordered_map<Key, HandleBase> map;
  mutable std::shared_mutex map_mutex;
  std::forward_list<Key> unused;
  typename std::forward_list<Key>::const_iterator unused_back =
      unused.before_begin();
  std::atomic<size_t> unused_size{0};
  std::mutex list_mutex;
  const size_t max_unused = 10;

  InitFunctor init_function;
};

} // namespace lru_cache

#endif // LRU_CACHE_LRUCACHE_H
