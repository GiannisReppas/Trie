A Trie Data Structure to manage words in-memory in the format of (word -> translation), effectively.

Trie data structure offers super-fast insert, search and delete operations.
Compression techniques like bit-mapping and RLE offer small memory footprint.

Trie manages characters as unsigned integers, currently offering types of uint8_t, uint16_t and uint32_t

The data structure can optionally load and save entries from disk binary and csv files.

A main function in ascii_example.cpp shows how to use a Trie of uint8_t characters to manage all ascii words.

To build and run:

mkdir build && cd build
cmake .. && make
./triectionary

Future plans:
1) allow more character sizes (diff. than 1/2/4 bytes)
2) Use a similar data-structure for the translation strings
3) More effective load-save operations on disk
4) switch between RLE and normal bitmapping when it is most optimal
5) add prefix, etc. functionality
6) python bindings
7) add real alphabet - integer mapping outside of the library
8) copy constructors
9) tests
10) iterative destructor, saver
11) trienode insert, trienode set_null and extreme cases error handling
12) end_of_string defined for every trie
13_ give dictionary name after trie creation