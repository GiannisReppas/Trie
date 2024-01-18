#include <fstream>
#include <vector>

#include "def.hpp"
#include "extended_character_functions.hpp"
#include "trie_node.hpp"
#include "trie.hpp"

namespace triectionary
{

Trie::Trie(std::string dictionary_name)
{
	// set up head node, 0 entries, dictionary name, saving_changes setting
	this->head = new TrieNode();
	this->entry_count = 0;
	this->dictionary_name = dictionary_name;
	this->saving_changes = false;

	// open dictionary file to read it
	FILE* file = fopen(this->dictionary_name.c_str(), "rb");
	if (file == NULL)
	{
		printf("Error opening file %s\n", this->dictionary_name.c_str());
		exit(-1);
	}

	// read character size for this dictionary
	uint32_t character_size;
	fread( &character_size, sizeof(uint32_t), 1, file);
	if (character_size != CHARACTER_BYTES)
	{
		printf("Dictionary can't be read - Different character size\n");
		exit(1);
	}

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
		current_word[word_size] = END_OF_STRING;

		// read translation
		fread( &word_size, sizeof(uint32_t), 1, file);
		current_translation = new character_t[word_size+1];
		fread( current_translation, sizeof(character_t), word_size, file);
		current_translation[word_size] = END_OF_STRING;

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
		file = fopen( this->dictionary_name.c_str(), "wb");
		if (file == NULL)
		{
			fprintf(stderr, "Error opening the file\n");
			exit(-1);
		}

		// write character size and total entries for this dictionary
		uint32_t character_size = CHARACTER_BYTES;
		fwrite( &character_size, sizeof(uint32_t), 1, file);

		fwrite( &this->entry_count, sizeof(uint32_t), 1, file);
	}

	//  start deleting nodes, recursively
	this->destroy_trie_node( this->head, std::vector<character_t>(), std::vector<character_t>(), file);

	if (this->saving_changes)
	{
		// close dictionary file
		fclose(file);
	}
}

void Trie::destroy_trie_node( TrieNode *to_destroy, std::vector<character_t> current_word, std::vector<character_t> letter_to_append, FILE* file)
{
	// append letter of path to current word
	current_word.insert(current_word.end(), letter_to_append.begin(), letter_to_append.end());

	if ( (this->saving_changes) && (to_destroy->translation != NULL) )
	{
		// write word and translation in dictionary file
		uint32_t word_size = current_word.size();
		fwrite( &word_size, sizeof(uint32_t), 1, file);
		fwrite( current_word.data(), sizeof(character_t), current_word.size(), file);
		word_size = strlen(to_destroy->translation);
		fwrite( &word_size, sizeof(uint32_t), 1, file);
		fwrite( to_destroy->translation, sizeof(character_t), strlen(to_destroy->translation), file);
	}

	// read zeros map
	// for every active letter that you find, call recursive destruction function
	character_t next_child = 0;
	character_t next_zeros_group = 0;
	for (character_t letter = 0; letter < TOTAL_ALPHABET_SIZE; letter++)
	{
		if ( (next_zeros_group < to_destroy->zeros_map_size) && (letter == to_destroy->zeros_map[next_zeros_group]) )
		{
			letter = to_destroy->zeros_map[next_zeros_group+1];
			if (letter == (TOTAL_ALPHABET_SIZE-1))
			{
				break;
			}
			letter++;
			next_zeros_group += 2;
		}

		this->destroy_trie_node( to_destroy->children[next_child++], current_word, std::vector<character_t>( 1, letter), file);
	}

	// at this point, you know that all children of this node are deleted
	// so, delete current node
	delete[] to_destroy->translation;
	delete[] to_destroy->zeros_map;
	delete[] to_destroy->children;
	delete to_destroy;
}

bool Trie::is_empty()
{
	return (this->head->children = NULL);
	//return (this->head->get_children_count() == 0);
}

character_t* Trie::search_word( const character_t* word)
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
		return NULL;

	return previous->translation;
}

character_t* Trie::add_word( const character_t* word, const character_t* translation)
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
	while ( word[current_word_position] != END_OF_STRING )
	{
		previous = previous->insert_letter( word[current_word_position] );
		++current_word_position;
	}

	// reached the end of the given word. Check if translation exists already and insert.
	if (previous->translation != NULL)
		return NULL;
	
	// add word with translation, increase entry_count
	previous->translation = new character_t[strlen(translation) + 1];
	strcpy( previous->translation, translation);

	this->entry_count++;

	return previous->translation;
}

character_t* Trie::delete_word( const character_t* word)
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
		return NULL;

	// at this point, you will surely have a successful deletion
	character_t* toReturn = new character_t[strlen(previous->translation) + 1];
	strcpy( toReturn, previous->translation);

	// delete translation
	delete[] previous->translation;
	previous->translation = NULL;

	// loop through the delete path in reverse order (same as looping through the word letters)
	for (int i=strlen(word); i > -1; i--)
	{
		/* if the node is in the delete path and
			1) doesn't have children (empty)
			2) doesn't have a translation
			3) is not the head of the trie
		   ,then delete it and inform its parent about the deletion
		   ,otherwise, end of deletion process */

		if ( (delete_path[i]->children == NULL) && (delete_path[i]->translation == NULL) && (delete_path[i] != this->head) )
		{
			delete[] delete_path[i]->zeros_map;
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

void Trie::insert_from_csv( std::string filename)
{
	/* the csv file needs to have the format: word,translation
		in case of multiple commas, the 1st one is chosen as a separator */

	// open the csv file
	std::ifstream cvs_file(filename);
	if (!cvs_file.is_open())
	{
		printf("error - cannot open file %s\n", filename.c_str());
		exit(-1);
	}

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
			arg1 = str_to_c(word);
			arg2 = str_to_c(translation);
			this->add_word( arg1, arg2 );
			delete[] arg1;
			delete[] arg2;
		}
	}

	// close csv file
	cvs_file.close();
}

void Trie::delete_from_csv( std::string filename)
{
	/* the csv file needs to have the format: word,translation
		in case of multiple commas, the 1st one is chosen as a separator */

	// open the csv file
	std::ifstream cvs_file(filename);
	if (!cvs_file.is_open())
	{
		printf("error - cannot open file %s\n", filename.c_str());
		exit(-1);
	}

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
			//translation = line.substr( comma_pos+1, line.size()-comma_pos);

			// delete tuple
			arg1 = str_to_c(word);
			arg2 = this->delete_word( arg1 );
			delete[] arg1;
			delete[] arg2;
		}
	}

	// close csv file
	cvs_file.close();
}

}