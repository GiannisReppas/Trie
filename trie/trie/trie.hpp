#ifndef TRIE_TRIE_H_
#define TRIE_TRIE_H_

#include <fstream>
#include <vector>
#include <stdint.h>
#include <limits>
#include <string>
#include <type_traits>

#include "trie/exceptions.hpp"
#include "trie/trie_node.hpp"

namespace trie
{

template <class character_t>
class Trie
{
private:
	/* name of the Trie (and the file in disk) */
	std::string dictionary_name;

	/* pointer to the head trie node of the Trie */
	TrieNode<character_t>* head;

	/* number of (word -> translation) pairs in the Trie */
	uint32_t entry_count;

public:
	Trie();
	Trie( std::string dictionary_name);
	~Trie();

	/* return true if Trie has 0 translations saved */
	bool is_empty();

	/* return number of saved translations */
	uint32_t get_entry_count();

	/* search for the translation of a word in the Trie
		return a pointer to the translation of the word
		return NULL if the word given doesn't exist in the Trie */
	character_t* search_word( const character_t* word);

	/* add a new word with its translation in the Trie
		return false if the word given already exists in the Trie */
	bool add_word( const character_t* word, const character_t* translation);

	/* delete a word from the Trie with its translation
		return false if the word given doesn't exist in the Trie */
	bool delete_word( const character_t* word);

	/* write current information of trie in the binary dictionary file */
	void save_changes();
	
	/* functions used to insert and delete pairs of (word,translation)
		insert can be used to import an already existing dictionary .csv file
		delete is used mainly for debugging and ignored the provided translation */
	void insert_from_csv( std::string filename);
	void delete_from_csv( std::string filename);

};

template <class character_t>
Trie<character_t>::Trie()
{
	// check if the type given is valid for the template class
	uint32_t bytes;
	if (std::is_same<character_t, uint8_t>::value)
		bytes = 1;
	else if (std::is_same<character_t, uint16_t>::value)
		bytes = 2;
	else if (std::is_same<character_t, uint32_t>::value)
		bytes = 4;
	else
		throw ErrorCreatingTrieException();

	// 0 entries, dictionary name empty
	this->entry_count = 0;
	this->dictionary_name = "";

	// set up head node
	this->head = new TrieNode<character_t>();
}

template <class character_t>
Trie<character_t>::Trie( std::string dictionary_name)
{
	// check if the type given is valid for the template class
	uint32_t bytes;
	if (std::is_same<character_t, uint8_t>::value)
		bytes = 1;
	else if (std::is_same<character_t, uint16_t>::value)
		bytes = 2;
	else if (std::is_same<character_t, uint32_t>::value)
		bytes = 4;
	else
		throw ErrorCreatingTrieException();

	// 0 entries, dictionary name
	this->entry_count = 0;
	this->dictionary_name = dictionary_name;

	// open dictionary file to read it
	uint32_t character_size;
	FILE* file = fopen(this->dictionary_name.c_str(), "rb");
	if (file == NULL)
	{
		file = fopen(this->dictionary_name.c_str(), "wb");
		if (file == NULL)
			throw ErrorOpeningDictionaryException(this->dictionary_name);

		character_size = sizeof(character_t);
		fwrite( &character_size, sizeof(uint32_t), 1, file);

		fwrite( &this->entry_count, sizeof(uint32_t), 1, file);

		fclose(file);

		file = fopen(this->dictionary_name.c_str(), "rb");
		if (file == NULL)
			throw ErrorOpeningDictionaryException(this->dictionary_name);
	}

	// read character size for this dictionary
	fread( &character_size, sizeof(uint32_t), 1, file);
	if (character_size != bytes)
	{
		fclose(file);
		throw ErrorReadingDictionaryException( this->dictionary_name, "Conflicting Trie and File types");
	}

	// set up head node
	this->head = new TrieNode<character_t>();

	// read total number of entries to insert in the trie
	uint32_t local_entry_count;
	fread( &local_entry_count, sizeof(uint32_t), 1, file);

	// read and add entries
	uint32_t word_size;
	character_t* current_word;
	character_t* current_translation;
	for (uint32_t i=0; i < local_entry_count; i++)
	{
		// read word
		fread( &word_size, sizeof(uint32_t), 1, file);
		current_word = new character_t[word_size+1];
		fread( current_word, sizeof(character_t), word_size, file);
		current_word[word_size] = ::trie::end_of_string;

		// read translation
		fread( &word_size, sizeof(uint32_t), 1, file);
		current_translation = new character_t[word_size+1];
		fread( current_translation, sizeof(character_t), word_size, file);
		current_translation[word_size] = ::trie::end_of_string;

		// add tuple
		this->add_word( current_word, current_translation);

		// prepare for next word
		delete[] current_word;
		delete[] current_translation;
	}

	// close dictionary file
	fclose(file);
}

template <class character_t>
Trie<character_t>::~Trie()
{
	//  start deleting nodes from head, recursively
	delete this->head;
}

template <class character_t>
bool Trie<character_t>::is_empty()
{
	return this->head->is_empty();
}

template <class character_t>
character_t* Trie<character_t>::search_word( const character_t* word)
{
	// read existing Trie until you reach unsaved part of the word
	// for a successful search, we should not have an unsaved part
	TrieNode<character_t>* current = this->head;
	TrieNode<character_t>* previous = NULL;
	uint32_t current_word_position = -1;
	while (current != NULL)
	{
		++current_word_position;

		previous = current;
		current = current->get_node_if_possible( word[current_word_position] );
	}

	// report an error if word given is not saved or it doesn't have a translation
	if ( (strlen(word) != current_word_position) || (previous->get_translation() == NULL) )
		return NULL;

	return previous->get_translation();
}

template <class character_t>
bool Trie<character_t>::add_word( const character_t* word, const character_t* translation)
{
	// read existing Trie until you reach unsaved part of the word
	TrieNode<character_t>* current = this->head;
	TrieNode<character_t>* previous = NULL;
	uint32_t current_word_position = -1;
	while (current != NULL)
	{
		++current_word_position;

		previous = current;
		current = current->get_node_if_possible( word[current_word_position] );
	}

	// start inserting TrieNodes (letters)
	while ( word[current_word_position] != ::trie::end_of_string )
	{
		previous = previous->insert_letter( word[current_word_position] );
		++current_word_position;
	}

	// reached the end of the given word. Check if translation exists already and insert.
	if (previous->get_translation() != NULL)
		return false;

	// add word with translation, increase entry_count
	previous->set_translation(translation);
	this->entry_count++;

	return true;
}

template <class character_t>
bool Trie<character_t>::delete_word( const character_t* word)
{
	// keep track of all the visited nodes while traversing the trie in an array of pointers
	// they could potentially be deleted in the end
	TrieNode<character_t>** delete_path = new TrieNode<character_t>*[ strlen(word)+1 ];
	for (uint32_t i=0; i < strlen(word); i++)
		delete_path[i] = NULL;

	// read existing Trie and update the delete path until you reach unsaved part of the word
	// for a successful deletion, we should not have an unsaved part
	TrieNode<character_t>* current = this->head;
	TrieNode<character_t>* previous = NULL;
	uint32_t current_word_position = -1;
	while (current != NULL)
	{
		++current_word_position;

		previous = current;
		current = current->get_node_if_possible( word[current_word_position] );

		delete_path[current_word_position] = previous;
	}

	// report an error if word given is not saved or it doesn't have a translation
	if ( (strlen(word) != current_word_position) || previous->get_translation() == NULL)
		return false;

	// at this point, you will surely have a successful deletion, delete translation
	previous->set_translation(NULL);

	// loop through the delete path in reverse order
	for (int i=strlen(word); i > -1; i--)
	{
		/* if current node in the delete path
			1) doesn't have children and translation (empty)
			2) is not the head of the trie
		   ,then delete it and inform its parent about the deletion
		   ,otherwise, end of deletion process */

		if ( (delete_path[i]->is_empty()) && (delete_path[i] != this->head) )
		{
			delete delete_path[i];
			delete_path[i-1]->set_child_null( word[i-1] );
		}
		else
		{
			break;
		}
	}

	// decrease the entry count by 1
	this->entry_count--;

	// delete all saved delete path pointers
	delete[] delete_path;

	return true;
}

template <class character_t>
uint32_t Trie<character_t>::get_entry_count()
{
	return this->entry_count;
}

template <class character_t>
void Trie<character_t>::save_changes()
{
	if (this->dictionary_name == "")
		return;

	// open dictionary file to write from scratch
	FILE* file = fopen( this->dictionary_name.c_str(), "wb");
	if (file == NULL)
		throw ErrorOpeningDictionaryException(this->dictionary_name);

	// write character size and total entries for this dictionary
	uint32_t character_size = sizeof(character_t);
	fwrite( &character_size, sizeof(uint32_t), 1, file);

	fwrite( &this->entry_count, sizeof(uint32_t), 1, file);

	// start saving tuples, recursively
	this->head->save_subtrie( std::vector<character_t>(), std::vector<character_t>(), file);

	// close dictionary file
	fclose(file);
}

template <class character_t>
void Trie<character_t>::insert_from_csv( std::string filename)
{
	/* the csv file needs to have the format: word,translation
		lines without a comma character are ignored
		in case of multiple commas, the 1st one is chosen as a separator */

	// open the csv file
	std::ifstream cvs_file(filename);
	if (!cvs_file.is_open())
		throw ErrorOpeningCsvException(filename);

	// read file line-by-line
	std::string word, translation;
	std::string line;
	character_t *arg1, *arg2;
	while (std::getline(cvs_file, line))
	{
		auto comma_pos = line.find(",");

		// check if comma exists in the line
		if (comma_pos != line.npos)
		{
			word = line.substr( 0, comma_pos);
			translation = line.substr( comma_pos+1, line.size()-comma_pos);

			// add tuple
			arg1 = str_to_c<character_t>(word);
			arg2 = str_to_c<character_t>(translation);
			this->add_word( arg1, arg2 );
			delete[] arg1;
			delete[] arg2;
		}
	}

	// close csv file
	cvs_file.close();
}

template <class character_t>
void Trie<character_t>::delete_from_csv( std::string filename)
{
	/* the csv file needs to have the format: word,translation
		lines without a comma character are ignored
		in case of multiple commas, the 1st one is chosen as a separator */

	// open the csv file
	std::ifstream cvs_file(filename);
	if (!cvs_file.is_open())
		throw ErrorOpeningCsvException(filename);

	// read file line-by-line
	std::string word, translation;
	std::string line;
	character_t *arg1;
	while (std::getline(cvs_file, line))
	{
		auto comma_pos = line.find(",");

		// check if comma exists in the line
		if (comma_pos != line.npos)
		{
			word = line.substr( 0, comma_pos);

			// delete tuple
			arg1 = str_to_c<character_t>(word);
			this->delete_word( arg1 );
			delete[] arg1;
		}
	}

	// close csv file
	cvs_file.close();
}

}

#endif
