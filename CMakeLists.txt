cmake_minimum_required(VERSION 3.10)
project(triectionary VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(triectionary ascii_example.cpp)

add_subdirectory(trie)
target_link_libraries(triectionary PUBLIC trie)
target_include_directories(triectionary PUBLIC ./trie/)
