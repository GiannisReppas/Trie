A Trie Data Structure to manage words in-memory in the format of (word -> translation), effectively.

Trie data structure offers super-fast insert, search and delete operations.
Compression techniques like bit-mapping and RLE offer small memory footprint.

Trie manages characters as unsigned integers, currently offering types of uint8_t, uint16_t and uint32_t

The data structure can optionally load and save entries from disk binary and csv files.

A main function in ascii_example.cpp shows how to use a Trie of uint8_t characters to manage all ascii words, or an extension of them, considering 256 different bits.

maximum number of entry count set to 2^64 - 1
maximum word size set to 2^8 - 2
maximum translation size set to 2^16 - 2

To build and run:

mkdir build && cd build
cmake .. && make
./triectionary

Future plans:
1) allow more character sizes (diff. than 1/2/4 bytes)
2) Use a similar data-structure for the translation strings
3) More effective load-save operations on disk
4) switch between RLE and normal bitmapping when it is most optimal
5) python bindings
6) add real alphabet - integer mapping example outside of the library
7) copy constructors
8) tests
9) iterative destructor, saver, prefixer
10) allow custom setting for word_size, translation_size, entry_count max. values for each trie
11) give dictionary name after trie creation
12) memory reallocations in batches, not 1-by-1
13) random word generation