#ifndef LRU_CACHE_LOG
#define LRU_CACHE_LOG

#ifdef LRU_CACHE_DEBUG

#include <iostream>

namespace lru_cache {
static auto &log = std::cerr;
}

#else // LRU_CACHE_DEBUG

namespace lru_cache {

namespace detail {
struct DiscardStream {};

template <typename T>
constexpr DiscardStream &operator<<(DiscardStream &str, const T &) {
  return str;
}
} // namespace detail

static detail::DiscardStream log;

} // namespace lru_cache

#endif // LRU_CACHE_DEBUG

#endif // LRU_CACHE_LOG
