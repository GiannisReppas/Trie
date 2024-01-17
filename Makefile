all: extended_character_functions.o trie_node.o trie.o main.o
	g++ main.o trie.o trie_node.o extended_character_functions.o -o my_dic.exe

main.o: trie/def.hpp trie/trie.hpp trie/trie_node.hpp trie/extended_character_functions.hpp
	g++ -c main.cpp

trie.o: trie/def.hpp trie/trie.hpp trie/trie_node.hpp trie/extended_character_functions.hpp
	g++ -c trie/trie.cpp

trie_node.o: trie/def.hpp trie/trie_node.hpp
	g++ -c trie/trie_node.cpp

extended_character_functions.o: trie/def.hpp trie/extended_character_functions.hpp
	g++ -c trie/extended_character_functions.cpp

clean:
	rm -f *.o
	rm -f *.exe
