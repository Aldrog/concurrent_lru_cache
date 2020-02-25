#ifndef LRU_CACHE_LOG
#define LRU_CACHE_LOG

#ifdef LRU_CACHE_DEBUG

#include <iostream>

static auto &log = std::cerr;

#else // LRU_CACHE_DEBUG

struct DiscardStream {};

template <typename T>
constexpr DiscardStream &operator<<(DiscardStream &str, const T &) {
  return str;
}

static DiscardStream log;

#endif // LRU_CACHE_DEBUG

#endif // LRU_CACHE_LOG
