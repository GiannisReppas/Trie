#ifndef TRIECTIONARY_TRIE_NODE_H_
#define TRIECTIONARY_TRIE_NODE_H_

#include "def.hpp"

namespace triectionary
{

typedef class TrieNode
{
public:
	/* variable size ( 2 to ((TOTAL_ALPHABET_SIZE/2) * 2) * sizeof(character_t) )
		worst case when bits have the form of 01010101... zeros_map add 2 elements for
		each 0 bit */
	character_t *zeros_map;
	uint32_t zeros_map_size;

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

	/* returns size of array children */
	uint32_t get_children_count();

	/* returns a Trienode pointer following the path of the argument letter, if there exists one */
	TrieNode* get_node_if_possible( character_t );

	/* adds a new Trienode path in current Trienode, updates both zeros_map and children
		assumes that letter given as argument will always be 0 in current zeros_map */
	TrieNode* insert_letter( character_t );
	
	/* deletes a Trienode path in current Trienode, updates both zeros_map and children
		assumes that letter given as argument will always be 1 in current zeros_map */
	void set_child_null( character_t );

}TrieNode;

}

#endif