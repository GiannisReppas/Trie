#ifndef TRIECTIONARY_TRIE_NODE_H_
#define TRIECTIONARY_TRIE_NODE_H_

#include "def.hpp"

namespace triectionary
{

typedef class TrieNode
{
public:
	/* variable size ( 0 to ((TOTAL_ALPHABET_SIZE/2) * 2) * sizeof(character_t) )
		worst case when bits have the form of 01010101... active_letters add 2 elements for
		each 0 bit */
	character_t *active_letters;
	uint32_t active_letters_size;

	/* variable size (0 to TOTAL_ALPHABET_SIZE*sizeof(pointer)) bytes
	 pointer usually 8 bytes */
	class TrieNode **children;

	/* variable size, 0 to translation_size*sizeof(character_size)+sizeof(character_size) bytes
	 uint8_t always 1 byte
	 uint16_t always 2 bytes
	 char32_t always 4 bytes */
	character_t *translation;

	TrieNode();
	~TrieNode();

	uint32_t get_children_count();

	TrieNode* get_node_if_possible( character_t );
	TrieNode* insert_letter( character_t );
	void set_child_null( character_t );

}TrieNode;

}

#endif