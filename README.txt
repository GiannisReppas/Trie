A Trie Data Structure to manage words in-memory in the format of (word -> translation) effectively.

Trie data structure offers super-fast insert, search and delete operations.
Compression techniques like bit-mapping and RLE result in small memory load of the data structure.

Future plans:
1) cmake compilation
2) create Trienodes of diff types in the same program
3) custom characters size (diff. than 1/2/4 bytes)
4) Use a similar data-structure for the translation strings
5) More effective load-save operations on disk
6) python bindings
7) switch between RLE and normal bitmapping when it is most optimal
8) add real alphabet - integer mapping outside of the library
9) add prefix, etc. functionality
