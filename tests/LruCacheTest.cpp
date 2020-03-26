#define LRU_CACHE_DEBUG
#include <lru_cache/LruCache.h>

#include <string>

#include <gtest/gtest.h>

using namespace lru_cache;

class LruCacheDefaultTest : public testing::Test {
public:
  LruCacheDefaultTest() = default;

protected:
  static constexpr size_t cache_size = 10;
  LruCache<int, std::string> cache{cache_size};

  void fill(int start) {
    for (int i = start; i < start + cache_size; ++i)
      cache.emplace(i, "test");
  }
};

TEST_F(LruCacheDefaultTest, emplace) {
  cache.emplace(-1, "hello");
  ASSERT_TRUE(cache.contains(-1));
  ASSERT_EQ(*cache.at(-1), "hello");
}

TEST_F(LruCacheDefaultTest, fill) {
  cache.emplace(-1, "hello");
  fill(0);
  ASSERT_FALSE(cache.contains(-1));
  ASSERT_THROW(cache.at(-1), std::exception);
}

TEST_F(LruCacheDefaultTest, reference) {
  cache.emplace(-1, "hello");
  auto hello = cache.at(-1);
  fill(0);
  ASSERT_TRUE(cache.contains(-1));
  ASSERT_EQ(cache.at(-1), hello);
  ASSERT_EQ(*cache.at(-1), "hello");
}

TEST_F(LruCacheDefaultTest, brackets) {
  *cache[-1] = "hello";
  fill(0);
  ASSERT_FALSE(cache.contains(-1));
  ASSERT_THROW(cache.at(-1), std::exception);
  ASSERT_NO_THROW(cache[-1]);
  auto hello = cache[-1];
  fill(cache_size);
  ASSERT_TRUE(cache.contains(-1));
  ASSERT_EQ(cache[-1], hello);
  ASSERT_NE(*cache[-1], "hello");
}

class LruCacheCustomInitTest : public testing::Test {
public:
  LruCacheCustomInitTest() = default;

protected:
  static constexpr size_t cache_size = 10;
  static constexpr auto initializer = [](int key) {
    if (key < 0)
      throw std::invalid_argument{""};
    return std::to_string(key);
  };
  LruCache<int, std::string, decltype(initializer)> cache{cache_size,
                                                          initializer};

  void fill(int start) {
    for (int i = start; i < start + cache_size; ++i)
      cache[i];
  }
};

TEST_F(LruCacheCustomInitTest, reference) {
  auto test = cache[0];
  fill(1);
  ASSERT_TRUE(cache.contains(0));
  ASSERT_EQ(cache.at(0), test);
  ASSERT_EQ(*test, "0");
}

TEST_F(LruCacheCustomInitTest, invalidKey) {
  ASSERT_THROW(cache[-1], std::invalid_argument);
}
