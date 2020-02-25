cmake_minimum_required(VERSION 3.13)

project(lru_cache LANGUAGES CXX)

add_library(lru_cache INTERFACE)
target_include_directories(lru_cache INTERFACE include/)
set_target_properties(lru_cache PROPERTIES INTERFACE_COMPILE_FEATURES cxx_std_17)

set(BUILD_LRU_CACHE_TESTS ON CACHE BOOL "Build lru_cache tests")
if(BUILD_LRU_CACHE_TESTS)
  add_subdirectory(tests)
endif()
