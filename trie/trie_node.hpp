#ifndef TRIECTIONARY_TRIE_NODE_H_
#define TRIECTIONARY_TRIE_NODE_H_

#include "def.hpp"

namespace triectionary
{

typedef class TrieNode
{
public:
	/* stable size of (TOTAL_ALPHABET_SIZE/32)*sizeof(uint32_t) bytes
	 uint8_t always 1 byte,
	 compilation fails if TOTAL_ALPHABET_SIZE isn't multiple of 32 */
	uint32_t active_letters[TOTAL_ALPHABET_SIZE/32];

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

	int get_actives_count();

	TrieNode* get_node_if_possible( character_t );
	TrieNode* insert_letter( character_t );
	void set_child_null( character_t );

}TrieNode;

}

#endif