cmake_minimum_required ( VERSION 3.10 )
project ( trie )
set ( CMAKE_CXX_STANDARD 11 )
set ( CMAKE_CXX_STANDARD_REQUIRED True )

add_library ( trie INTERFACE )
target_include_directories ( trie INTERFACE ./trie/ )

include_directories(./trie/)