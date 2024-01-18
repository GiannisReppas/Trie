#ifndef TRIECTIONARY_TRIE_H_
#define TRIECTIONARY_TRIE_H_

#include "def.hpp"
#include "trie_node.hpp"

namespace triectionary
{

typedef class Trie
{
private:
	/* pointer to the head trie node of the Trie */
	TrieNode* head;

	/* number of (word -> translation) pairs in the Trie */
	uint32_t entry_count;

	/* name of the Trie (and the file in disk) */
	std::string dictionary_name;

	/* if its true, then the trie will be saved in disk (file) at destruction */
	bool saving_changes;

public:
	Trie( std::string);
	~Trie();
	void destroy_trie_node( TrieNode*, std::vector<character_t> current_word, std::vector<character_t> append_instantly, FILE*);

	/* return true if the trie is empty (0 (word -> translations) paris saved) */
	bool is_empty();

	/* Search a word in trie. Return a pointer to the saved translation
		Returns NULL in case the word is not found */
	character_t* search_word( const character_t*);

	/* Add a word in the trie. Return a pointer to the saved translation
		Returns NULL in case the word already has a saved translation */
	character_t* add_word( const character_t*, const character_t*);

	/* Delete a word from trie. Return a pointer to the deleted translation.
		Returned translation should be freed by the caller
		Returns NULL in case the word is not found */
	character_t* delete_word( const character_t*);

	/* Return number of (word -> translation) pairs saved currently in the trie */
	uint32_t get_entry_count();

	/* set if the trie will be saved in the file "dictionary_name" at destruction
		default: false */
	void set_saving_changes( bool );

	/* functions used to insert and delete pairs of (word,translation)
		insert can be used to import an already existing dictionary .csv file
		delete is used mainly for debugging and ignored the provided translation */
	void insert_from_csv( std::string );
	void delete_from_csv( std::string );
}Trie;

}

#endif