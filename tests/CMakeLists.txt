set(BUILD_GTEST ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
add_subdirectory(dependencies/googletest)

add_executable(lru_cache_tests LruCacheTest.cpp)
target_link_libraries(lru_cache_tests lru_cache gtest_main)
