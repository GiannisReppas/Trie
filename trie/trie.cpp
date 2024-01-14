#include <fstream>
#include <cstring>
#include <climits>

#include "trie.hpp"

Trie::Trie(const char* dictionary_name)
{
	// set up head node, 0 entries, dictionary name, saving_changes setting
	this->saving_changes = false;
	this->entry_count = 0;
	this->dictionary_name = dictionary_name;
	this->head = new TrieNode();

	// open dictionary file to read it
	FILE* file = fopen(this->dictionary_name, "rb");
	if (file == NULL)
	{
		fprintf(stderr, "Error opening file %s\n", this->dictionary_name);
		exit(-1);
	}

	// read total number of entries to insert in the trie
	uint32_t local_entry_count;
	fread( &local_entry_count, sizeof(uint32_t), 1, file);

	// read and add entries
	uint32_t word_size;
	char* current_word;
	char* current_translation;
	for (uint32_t i=0; i < local_entry_count; i++)
	{
		// read word
		fread( &word_size, sizeof(uint32_t), 1, file);
		current_word = new char[word_size+1];
		fread( current_word, sizeof(char), word_size, file);
		current_word[word_size] = '\0';

		// read translation
		fread( &word_size, sizeof(uint32_t), 1, file);
		current_translation = new char[word_size+1];
		fread( current_translation, sizeof(char), word_size, file);
		current_translation[word_size] = '\0';

		// add tuple
		this->add_word( current_word, current_translation);

		// prepare for next word
		delete[] current_word;
		delete[] current_translation;
	}

	// close dictionary file
	fclose(file);
}

Trie::~Trie()
{
	FILE* file;

	if (this->saving_changes)
	{
		// open dictionary file to write from scratch
		file = fopen( this->dictionary_name, "wb");
		if (file == NULL)
		{
			fprintf(stderr, "Error opening the file\n");
			exit(-1);
		}
	}

	if (this->saving_changes)
		// write total entries in the trie
		fwrite( &this->entry_count, sizeof(uint32_t), 1, file);

	//  start writing and deleting nodes, recursively
	std::string current_word="";
	this->destroy_trie_node( this->head, current_word, file);

	if (this->saving_changes)
		// close dictionary file
		fclose(file);
}

void Trie::destroy_trie_node( TrieNode *to_destroy, std::string current_word, FILE* file)
{
	if ( (this->saving_changes) && (to_destroy->translation != NULL) )
	{
		// write word and translation in dictionary file
		uint32_t word_size = current_word.size();
		fwrite( &word_size, sizeof(uint32_t), 1, file);
		fwrite( &current_word[0], sizeof(char), current_word.size(), file);
		word_size = strlen(to_destroy->translation);
		fwrite( &word_size, sizeof(uint32_t), 1, file);
		fwrite( to_destroy->translation, sizeof(char), strlen(to_destroy->translation), file);
	}
	// remember to delete translation before deleting the node
	delete[] to_destroy->translation;

	// read active letters in current node
	// for every active letter that you find, call recursive destruction function
	unsigned char c;
	uint32_t next_child = 0;
	for ( uint32_t i = 0; i < (TOTAL_ALPHABET_SIZE/CHAR_BIT); i++)
	{
		c = to_destroy->active_letters[i];

		// we always assume that 1 byte contains 8 bits, so we do a small loop-unrolling
		if (c & 0b10000000)
			this->destroy_trie_node(
				to_destroy->children[next_child++],current_word + static_cast<char>(i*8+0), file);
		if (c & 0b01000000)
			this->destroy_trie_node(
				to_destroy->children[next_child++],current_word + static_cast<char>(i*8+1), file);
		if (c & 0b00100000)
			this->destroy_trie_node(
				to_destroy->children[next_child++],current_word + static_cast<char>(i*8+2), file);
		if (c & 0b00010000)
			this->destroy_trie_node(
				to_destroy->children[next_child++],current_word + static_cast<char>(i*8+3), file);
		if (c & 0b00001000)
			this->destroy_trie_node(
				to_destroy->children[next_child++],current_word + static_cast<char>(i*8+4), file);
		if (c & 0b00000100)
			this->destroy_trie_node(
				to_destroy->children[next_child++],current_word + static_cast<char>(i*8+5), file);
		if (c & 0b00000010)
			this->destroy_trie_node(
				to_destroy->children[next_child++],current_word + static_cast<char>(i*8+6), file);
		if (c & 0b00000001)
			this->destroy_trie_node(
				to_destroy->children[next_child++],current_word + static_cast<char>(i*8+7), file);
	}

	// at this point, you know that all children of this node are deleted
	// so, delete current node
	delete[] to_destroy->children;
	delete to_destroy;
}

bool Trie::is_empty()
{
	return (this->head->children = NULL);
	//return (this->head->get_actives_count() == 0);
}

std::string Trie::add_word( const char* word, const char* translation)
{
	TrieNode* current = this->head;
	TrieNode* previous = NULL;
	uint32_t current_word_position = -1;

	// read existing Trie until you reach unsaved part of the word
	while (current != NULL)
	{
		++current_word_position;

		previous = current;
		current = current->get_node_if_possible( word[current_word_position] );
	}

	// start inserting TrieNodes (letters)
	while ( word[current_word_position] != '\0' )
	{
		previous = previous->insert_letter( word[current_word_position] );
		++current_word_position;
	}

	// reached the end of the given word. Check if translation exists already and insert.
	if (previous->translation != NULL)
		return "";
	
	// add word with translation, increase entry_count
	previous->translation = new char[strlen(translation) + 1];
	strcpy( previous->translation, translation);
	this->entry_count++;

	return std::string( translation );;
}

std::string Trie::search_word( const char* word)
{
	TrieNode* current = this->head;
	TrieNode* previous = NULL;
	uint32_t current_word_position = -1;

	// read existing Trie until you reach unsaved part of the word
	// for a successful search, we should not have an unsaved part
	while (current != NULL)
	{
		++current_word_position;

		previous = current;
		current = current->get_node_if_possible( word[current_word_position] );
	}

	// report an error if word given is not saved or it doesn't have a translation
	if ( (strlen(word) != current_word_position) || previous->translation == NULL)
		return "";

	return std::string( previous->translation );
}

std::string Trie::delete_word( const char* word)
{
	// keep track of all the visited nodes while traversing the trie in an array of pointers
	// they could potentially be deleted in the end
	TrieNode** delete_path = new TrieNode*[ strlen(word)+1 ];
	for (uint32_t i=0; i < strlen(word); i++)
		delete_path[i] = NULL;

	TrieNode* current = this->head;
	TrieNode* previous = NULL;
	uint32_t current_word_position = -1;

	// read existing Trie and update the delete path until you reach unsaved part of the word
	// for a successful deletion, we should not have an unsaved part
	while (current != NULL)
	{
		++current_word_position;

		previous = current;
		current = current->get_node_if_possible( word[current_word_position] );

		delete_path[current_word_position] = previous;
	}

	// report an error if word given is not saved or it doesn't have a translation
	if ( (strlen(word) != current_word_position) || previous->translation == NULL)
		return "";

	// at this point, you will surely have a successful deletion
	std::string toReturn(previous->translation);

	// delete translation
	delete[] previous->translation;
	previous->translation = NULL;

	// loop through the delete path in reverse order (same as looping through the word letters)
	for (int i=strlen(word); i > -1; i--)
	{
		// if the node in the delete path
		// 	1) doesn't have children (empty)
		//  2) doesn't have a translation
		//  3) is not the head of the trie
		// , then delete it
		// , otherwise, end of deletion process

		if ( (delete_path[i]->children == NULL) &&
			(delete_path[i]->translation == NULL) && (this->head != delete_path[i]) )
		{
			delete delete_path[i];
			delete_path[i-1]->set_child_null( word[i-1] );
		}
		else
		{
			break;
		}
	}

	// decrease the entry count by 1s
	this->entry_count--;

	// delete all saved delete path
	delete[] delete_path;

	return toReturn;
}

uint32_t Trie::get_entry_count()
{
	return this->entry_count;
}

void Trie::set_saving_changes(bool decision)
{
	this->saving_changes = decision;
}

void Trie::import_tsv( std::string filename)
{
	/* the tsv file needs to have the format:
		word 'space' translation */

	// open the tsv file
	std::string word, translation;
	std::ifstream inp(filename);
	if (!inp)
	{
		printf("error - cannot open file %s", filename.c_str());
		exit(-1);
	}

	// read file line-by-line
	while (inp >> word >> translation)
	{
		// add tuple
		this->add_word( word.c_str(), translation.c_str());
	}

	// close tsv file
	inp.close();
}