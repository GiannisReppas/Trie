A Trie Data Structure to manage words in-memory in the format of (word -> translation), effectively.

Requirements: cmake 3.10 or higher

Trie data structure offers super-fast insert, search and delete operations.
Compression techniques like bit-mapping and RLE offer small memory footprint.

Trie manages characters as unsigned integers, currently offering types of uint8_t, uint16_t and uint32_t

The data structure can optionally load and save entries from disk binary and csv files.

A main function in ascii_example.cpp shows how to use a Trie of uint8_t characters to manage all ascii words (or an extension of them, considering 256 different character).

maximum number of entry count set to 2^64 - 1
maximum word size set to 2^8 - 2
maximum translation size set to 2^16 - 2

To build and run:

mkdir build && cd build
cmake .. && make
./triectionary

---------- Future Plans ----------

UI related trie functions:
1) random word generation
2) provide real alphabet - integer mapping example outside of the library
3) translation search (complicated, linked to core implementation)
4) modify translation (instead of deleting and inserting)
5) print all entries (maybe in user-defined batches)

Core implementation:
1) memory reallocations in batches, not 1-by-1
2) copy constructors
3) iterative destructor, saver, prefixer
4) switch between RLE and normal bitmapping when it is most optimal
5) allow custom setting for word_size, translation_size, entry_count max. values for each trie
6) More versatile and fast load-save operations on disk (e.g. change dictionary file, name after creation)
7) allow more character sizes (diff. than 1/2/4 bytes)

Other:
1) python bindings
2) tests
