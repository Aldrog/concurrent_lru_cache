#define LRU_CACHE_DEBUG
#include <lru_cache/LruCache.h>

#include <string>

#include <gtest/gtest.h>

using namespace lru_cache;

class LruCacheTest : public testing::Test {
public:
  LruCacheTest() = default;

protected:
  static constexpr size_t cache_size = 10;
  LruCache<int, std::string> cache{cache_size};

  void fill(int start) {
    for (int i = start; i < start + cache_size; ++i)
      cache.emplace(i, "test");
  }
};

TEST_F(LruCacheTest, add) {
  cache.emplace(-1, "hello");
  ASSERT_TRUE(cache.contains(-1));
  ASSERT_EQ(*cache.at(-1), "hello");
}

TEST_F(LruCacheTest, fill) {
  cache.emplace(-1, "hello");
  fill(0);
  ASSERT_FALSE(cache.contains(-1));
  ASSERT_THROW(cache.at(-1), std::exception);
}

TEST_F(LruCacheTest, reference) {
  cache.emplace(-1, "hello");
  auto hello = cache.at(-1);
  fill(0);
  ASSERT_TRUE(cache.contains(-1));
  ASSERT_EQ(cache.at(-1), hello);
  ASSERT_EQ(*cache.at(-1), "hello");
}
