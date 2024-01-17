#ifndef TRIECTIONARY_TRIE_H_
#define TRIECTIONARY_TRIE_H_

#include "def.hpp"
#include "trie_node.hpp"

namespace triectionary
{

typedef class Trie
{
private:
	TrieNode* head; // pointer to the head of the Trie
	uint32_t entry_count; // number of translations in the Trie
	std::string dictionary_name; // name of the Trie (and the file in disk)
	bool saving_changes; // if its true, then the trie will be saved in disk at destruction

public:
	Trie( std::string);
	~Trie();
	void destroy_trie_node( TrieNode*, std::vector<character_t> current_word, std::vector<character_t> append_instantly, FILE*);

	bool is_empty();

	character_t* add_word( const character_t*, const character_t*);
	character_t* search_word( const character_t*);
	character_t* delete_word( const character_t*);

	uint32_t get_entry_count();

	void set_saving_changes( bool );

	void import_csv( std::string );
}Trie;

}

#endif