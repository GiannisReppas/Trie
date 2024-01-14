#include <cstdint>
#include <string>

#include "trie_node.hpp"

/*------------------------------------------------------------------------------------------------*/

typedef class Trie
{
private:
	TrieNode* head; // pointer to the head of the Trie
	uint32_t entry_count; // number of translations in the Trie
	const char* dictionary_name; // name of the Trie (and the file in disk)
	bool saving_changes; // if its true, then the trie will be saved in disk at destruction

public:
	Trie( const char*);
	~Trie();
	void destroy_trie_node( TrieNode*, std::string, FILE*);

	bool is_empty();

	std::string add_word( const char*, const char*);
	std::string search_word( const char*);	
	std::string delete_word( const char*);

	uint32_t get_entry_count();

	void set_saving_changes( bool );

	void import_tsv( std::string );
}Trie;

/*------------------------------------------------------------------------------------------------*/