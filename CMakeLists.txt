cmake_minimum_required ( VERSION 3.10 )
project( trie_tests )
set ( CMAKE_CXX_STANDARD 11)
set ( CMAKE_CXX_STANDARD_REQUIRED True )

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin )

add_subdirectory( ./trie )
include_directories(./trie/)

option ( TRIECTIONARY_TEST "Build the Trie test suites."    ON )
option ( TRIE_BUILD_TESTS "Build the Trie test suites."     ON )

if ( TRIECTIONARY_TEST )
    add_executable ( triectionary ./test/triectionary/triectionary.cpp )
    target_link_libraries ( triectionary PUBLIC trie )
endif()

if ( TRIE_BUILD_TESTS )
    enable_testing()

    add_subdirectory(./test/gtest)
endif()
