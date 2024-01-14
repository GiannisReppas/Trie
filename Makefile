all: main.o trie.o trie_node.o
	g++ main.o trie.o trie_node.o -o my_dic.exe

main.o: trie/trie.hpp
	g++ -c main.cpp

trie.o: trie/trie.hpp trie/trie_node.hpp
	g++ -c trie/trie.cpp

trie_node.o: trie/trie_node.hpp
	g++ -c trie/trie_node.cpp

clean:
	rm -f *.o
	rm -f trie/*.o
	rm -f *.exe
