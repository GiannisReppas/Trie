#ifndef TRIE_TRIE_NODE_H_
#define TRIE_TRIE_NODE_H_

#include <stdint.h>
#include <limits>
#include <vector>

#include "trie/extended_character_functions.hpp"

namespace trie
{

template <class character_t> 
class TrieNode
{
private:
	// type to be used for the edge cases of having the biggest possible zeros_map
	using character_t_parent =
		typename std::conditional< sizeof(character_t) == 1, uint16_t,
		typename std::conditional< sizeof(character_t) == 2, uint32_t,
		typename std::conditional< sizeof(character_t) == 4, uint64_t,
		void>::type >::type >::type;

	/* variable size ( 2 to ((ALPHABET_SIZE/2) * 2) * sizeof(character_t) )
		worst case when bits have the form of 01010101... zeros_map add 2 elements for
		each 0 bit
		due to this case, zeros_map may have size larger than max. value of character_t
		e.g. size of 256 for character_t -> uint8_t
		as a result, we keep half of zeros_map size (for every pair that it keeps)
		that way, we save memory */
	character_t *zeros_map;
	character_t zeros_map_half_size;

	/* variable size (0 to ALPHABET_SIZE*sizeof(pointer)) bytes
		pointer usually 8 bytes
		We don't keep its size to save space. We get the size by reading zeros_map */
	class TrieNode **children;

	/* variable size, 0 to translation_size*sizeof(character_size)+sizeof(character_size) bytes */
	character_t *translation;

	/*
		uint8_t  always 1 byte
		uint16_t always 2 bytes
		char32_t always 4 bytes
	*/

public:
	TrieNode();
	~TrieNode();

	/* return true if TrieNode has 0 children and no translation */
	bool is_empty();

	/* return size of children array - return parent type always to catch worst case for character_t */
	character_t_parent get_children_count();

	/* manage TrieNode translation */
	character_t* get_translation();
	void set_translation(const character_t* translation);

	/* return a Trienode pointer following the path of the argument letter
		return NULL if there doesn't exist one */
	TrieNode* get_node_if_possible(const character_t letter );

	/* adds a new Trienode path in current Trienode, updates both zeros_map and children
		assumes that letter given as argument will always be 0 in current zeros_map
		return a pointer to the newly inserted child */
	TrieNode* insert_letter(const character_t letter );

	/* deletes a Trienode path in current Trienode, updates both zeros_map and children
		assumes that letter given as argument will always be 1 in current zeros_map */
	bool set_child_null(const character_t letter );

	/* write words with their translations of the sub-trie of current TrieNde in the file pointed by the file pointer */
	void save_subtrie( std::vector<character_t> current_word, std::vector<character_t> letter_to_append, FILE* file);

	/* get words that are saved in the Trie and start with given prefix (TrieNode subtrie) */
	bool get_prefix_words( character_t** toReturn, std::vector<character_t> current_word, std::vector<character_t> letter_to_append, uint8_t& count);
};

template <class character_t> 
TrieNode<character_t>::TrieNode()
{
	// 2 elements to depict a full zero group for all bits
	this->zeros_map = new character_t[2];
	this->zeros_map[0] = 0;
	this->zeros_map[1] = std::numeric_limits<character_t>::max();
	this->zeros_map_half_size = 1;

	// no children in the TrieNode
	this->children = NULL;

	// no translation in the TrieNode
	this->translation = NULL;
}

template <class character_t> 
TrieNode<character_t>::~TrieNode()
{
	// first, destruct all children of current node
	// loop through all possile letters
	// if current letter enters a zeros group, shift to end of the zeros group
	character_t_parent children_count = this->get_children_count();
	for (character_t_parent child = 0; child < children_count; child++)
	{
		delete this->children[child];
	}

	// all children deleted, so delete current TrieNode
	delete[] this->translation;
	delete[] this->zeros_map;
	delete[] this->children;
}

template <class character_t>
bool TrieNode<character_t>::is_empty()
{
	return ( (this->children == NULL) && (this->translation == NULL) );
}

template <class character_t> 
typename TrieNode<character_t>::character_t_parent TrieNode<character_t>::get_children_count()
{
	character_t_parent children_count = 0;

	// ones before the 1st zeros group
	children_count += this->zeros_map[0];

	// ones between the zeros groups
	for ( character_t i = 0; i < (this->zeros_map_half_size*2)-2; i += 2)
		children_count += this->zeros_map[i+2] - this->zeros_map[i+1] - 1;

	// ones after the last zeros group
	children_count += std::numeric_limits<character_t>::max() - this->zeros_map[(this->zeros_map_half_size*2)-1];

	return children_count;
}

template <class character_t>
character_t* TrieNode<character_t>::get_translation()
{
	return this->translation;
}

template <class character_t>
void TrieNode<character_t>::set_translation(const character_t* t)
{
	// delete old value
	delete[] this->translation;

	// set new value
	if (t != NULL)
	{
		this->translation = new character_t[strlen(t) + 1];
		strcpy( this->translation, t);
	}
	else
	{
		this->translation = NULL;
	}
}

template <class character_t>
TrieNode<character_t>* TrieNode<character_t>::get_node_if_possible(const character_t letter )
{
	character_t_parent children_count = 0;

	// letter exists before the 1st zeros group (ones group)
	if (letter < this->zeros_map[0])
		return this->children[ letter ];

	// update current children count
	children_count += this->zeros_map[0];

	for ( character_t i = 0; i < (this->zeros_map_half_size*2)-2; i += 2)
	{
		// letter exists in a zeros group
		if ( (this->zeros_map[i] <= letter) && (letter <= this->zeros_map[i+1]) )
			return NULL;

		// letter exists between 2 zeros groups (ones group)
		if ( (this->zeros_map[i+1] < letter) && (letter < this->zeros_map[i+2]) )
			return this->children[ children_count + (letter - this->zeros_map[i+1]) - 1 ];

		// update current children count
		children_count += this->zeros_map[i+2] - this->zeros_map[i+1] - 1;
	}

	// letter exists in the last zeros group
	character_t i = (this->zeros_map_half_size*2) - 2;
	if ( (this->zeros_map[i] <= letter) && (letter <= this->zeros_map[i+1]) )
		return NULL;

	// letter exists after the last zeros group (ones group)
	if (this->zeros_map[i+1] < letter)
		return this->children[ children_count + (letter - this->zeros_map[i+1]) - 1 ];

	// never to reach this point, report an error in case we reach it
	printf("Error while searching trie_node\n");
	return NULL;
}

template <class character_t>
TrieNode<character_t>* TrieNode<character_t>::insert_letter(const character_t letter )
{
	/* 1) First
			- count number of children pointers
			- find the index of zeros_map that will be changed
			- find the index at which the new pointer will be inserted in children array */

	character_t_parent children_count = 0;
	character_t index_to_insert_children, index_to_change_zeros;

	// update current children count
	children_count += this->zeros_map[0];

	for ( character_t i = 0; i < (this->zeros_map_half_size*2)-2; i += 2)
	{
		// found the zeros group that letter exists
		if ( (this->zeros_map[i] <= letter) && (letter <= this->zeros_map[i+1]) )
		{
			index_to_change_zeros = i;
			index_to_insert_children = children_count;
		}

		// update current children count
		children_count += this->zeros_map[i+2] - this->zeros_map[i+1] - 1;
	}

	// letter exists in the last zeros group
	if ( (this->zeros_map[(this->zeros_map_half_size*2)-2] <= letter) && (letter <= this->zeros_map[(this->zeros_map_half_size*2)-1]) )
	{
		index_to_change_zeros = (this->zeros_map_half_size*2)-2;
		index_to_insert_children = children_count;
	}

	// update current children count
	children_count += std::numeric_limits<character_t>::max() - this->zeros_map[(this->zeros_map_half_size*2)-1];


	/* 2) Now, create a new pointers array of size children_count+1,
			with the extra addition of the pointer for the letter received as argument */

	// Create the new TrieNode to return
	TrieNode* toReturn = new TrieNode<character_t>();

	// Create new children pointers array
	TrieNode **new_children = new TrieNode<character_t> *[children_count+1];

	for ( character_t i = 0; i < index_to_insert_children; i++)
		new_children[i] = this->children[i];

	new_children[index_to_insert_children] = toReturn;

	for ( character_t_parent i = index_to_insert_children+1; i < children_count+1; i++)
		new_children[i] = this->children[i-1];

	// swap with current children pointers array and delete the old one
	delete[] this->children;
	this->children = new_children;


	/* 3) Lastly, update the zeros map array
		there are 3 different cases : 1) e.g. for [8,13], make 10 one -> [8,9] and [11,13] (insert 2)
									 2a) e.g. for [8,9],  make 8  one -> [9,9] (modify 1)
									 2b) e.g. for [8,9],  make 9  one -> [8,8] (modify 1)
									  3) e.g. for [8,8],  make 8  one -> - (delete 2) */

	if ( (this->zeros_map[index_to_change_zeros] < letter) && (letter < this->zeros_map[index_to_change_zeros+1]) )
	{
		character_t *new_zeros;
		character_t old_end_value = this->zeros_map[index_to_change_zeros+1];
		this->zeros_map[index_to_change_zeros+1] = letter-1;
		new_zeros = new character_t[(this->zeros_map_half_size*2)+2];

		for (character_t i = 0; i < index_to_change_zeros+2; i++)
			new_zeros[i] = this->zeros_map[i];

		new_zeros[index_to_change_zeros+2] = letter+1;
		new_zeros[index_to_change_zeros+3] = old_end_value;

		for (character_t i = index_to_change_zeros+4; i < (this->zeros_map_half_size*2)+2; i++)
			new_zeros[i] = this->zeros_map[i-2];

		delete[] this->zeros_map;
		this->zeros_map = new_zeros;
		this->zeros_map_half_size += 1;
	}
	else if ( (letter == this->zeros_map[index_to_change_zeros]) && (letter != this->zeros_map[index_to_change_zeros+1]) )
	{
		this->zeros_map[index_to_change_zeros] += 1;
	}
	else if ( (letter != this->zeros_map[index_to_change_zeros]) && (letter == this->zeros_map[index_to_change_zeros+1]) )
	{
		this->zeros_map[index_to_change_zeros+1] -= 1;
	}
	else
	{
		character_t *new_zeros;
		new_zeros = new character_t[(this->zeros_map_half_size*2)-2];

		for (character_t i = 0; i < index_to_change_zeros; i++)
			new_zeros[i] = this->zeros_map[i];

		for (character_t_parent i = index_to_change_zeros+2; i < (this->zeros_map_half_size*2); i++)
			new_zeros[i-2] = this->zeros_map[i];

		delete[] this->zeros_map;
		this->zeros_map = new_zeros;
		this->zeros_map_half_size -= 1;
	}


	return toReturn;
}

template <class character_t>
bool TrieNode<character_t>::set_child_null(const character_t letter )
{
	/* 1) First
			- count number of children pointers
			- find the index of zeros_map that will be changed
			- find the index at which the pointer will be deleted from children array */

	character_t_parent children_count = 0;
	character_t index_to_delete_children, index_to_change_zeros;

	// letter exists before the 1st zeros group (ones group)
	// in this case, we don't set index_to_change_zeros, because it can't have a value of -1 (see how we handle it in step 3)
	if ( letter < this->zeros_map[0])
	{
		index_to_delete_children = letter;
	}

	// update current children count
	children_count += this->zeros_map[0];

	for ( character_t i = 0; i < (this->zeros_map_half_size*2)-2; i += 2)
	{
		// found the zeros group after which the letter exists
		if ( (this->zeros_map[i+1] < letter) && (letter < this->zeros_map[i+2]) )
		{
			index_to_change_zeros = i+1;
			index_to_delete_children = children_count + (letter - this->zeros_map[i+1]) - 1;
		}

		// update current children count
		children_count += this->zeros_map[i+2] - this->zeros_map[i+1] - 1;
	}

	// letter exists after the last zeros group
	if ( (this->zeros_map[(this->zeros_map_half_size*2)-1] < letter) )
	{
		index_to_change_zeros = (this->zeros_map_half_size*2)-1;
		index_to_delete_children = children_count + (letter - this->zeros_map[(this->zeros_map_half_size*2) - 1]) - 1;
	}

	// update current children count
	children_count += std::numeric_limits<character_t>::max() - this->zeros_map[(this->zeros_map_half_size*2)-1];


	/* 2) Now, create a new pointers array of size children_count-1,
			without the pointer for the letter received as argument */

	// Create new child pointers array
	TrieNode **temp;
	(children_count > 1) ? temp = new TrieNode<character_t> *[children_count-1] : temp = NULL;

	for ( character_t i = 0; i < index_to_delete_children; i++)
		temp[i] = this->children[i];

	for ( character_t_parent i = index_to_delete_children+1; i < children_count; i++)
		temp[i-1] = this->children[i];

	// swap with current child pointers array and delete the old one
	delete[] this->children;
	this->children = temp;


	/* 3) Lastly, update the zeros map array
		there are 5 different cases : 1) edge-case, 1 exists before zeros_map[0]
											if (letter == zeros_map[0]-1)
												e.g.  [5,x], make 4 zero  -> [4,x] (modify 1)
											else
												e.g.  [5,x], make 3 zero  -> [3,3] [5,x] (insert 2)
									  2) edge-case, 1 exists after zeros_map[(zeros_map_half_size*2)-1]
											if (letter == zeros_map[(zeros_map_half_size*2)-1]+1)
											   e.g.  [x,97], make 98 zero -> [x,98] (modify 1)
											else
											   e.g.  [x,97], make 99 zero -> [x,97] [99,99] (insert 2)
									  3) e.g. [x,60] [62,y], make 61 zero -> [x,y] (delete 2)
									 4a) e.g. [x,60] [70,y], make 61 zero -> [x,61] [70,y] (modify 1)
									 4b) e.g. [x,60] [70,y], make 69 zero -> [x,60] [69,y] (modify 1)
									  5) e.g. [x,60] [70,y], make 62 zero -> [x,61] [62,62] [70,y] (insert 2) */

	if (letter < this->zeros_map[0])
	{
		if (letter == this->zeros_map[0]-1)
		{
			this->zeros_map[0]--;
		}
		else
		{
			character_t *new_zeros;
			new_zeros = new character_t[(this->zeros_map_half_size*2)+2];

			new_zeros[0] = letter;
			new_zeros[1] = letter;

			for (character_t i = 0; i < (this->zeros_map_half_size*2); i++)
				new_zeros[i+2] = this->zeros_map[i];

			delete[] this->zeros_map;
			this->zeros_map = new_zeros;
			this->zeros_map_half_size += 1;
		}
	}
	else if (index_to_change_zeros == (this->zeros_map_half_size*2)-1)
	{
		if (letter == this->zeros_map[index_to_change_zeros] + 1)
		{
			this->zeros_map[index_to_change_zeros]++;
		}
		else
		{
			character_t *new_zeros;
			new_zeros = new character_t[(this->zeros_map_half_size*2)+2];

			for (character_t i = 0; i < (this->zeros_map_half_size*2); i++)
				new_zeros[i] = this->zeros_map[i];

			new_zeros[index_to_change_zeros+1] = letter;
			new_zeros[index_to_change_zeros+2] = letter;

			delete[] this->zeros_map;
			this->zeros_map = new_zeros;
			this->zeros_map_half_size += 1;
		}
	}
	else if ( (this->zeros_map[index_to_change_zeros+1] - this->zeros_map[index_to_change_zeros]) == 2)
	{
		this->zeros_map[index_to_change_zeros] = this->zeros_map[index_to_change_zeros+2];

		character_t *new_zeros;
		new_zeros = new character_t[(this->zeros_map_half_size*2)-2];

		for (character_t i = 0; i < index_to_change_zeros+1; i++)
			new_zeros[i] = this->zeros_map[i];

		for (character_t_parent i = index_to_change_zeros+3; i < (this->zeros_map_half_size*2); i++)
			new_zeros[i-2] = this->zeros_map[i];

		delete[] this->zeros_map;
		this->zeros_map = new_zeros;
		this->zeros_map_half_size -= 1;
	}
	else if ( (this->zeros_map[index_to_change_zeros]+1 == letter) )
	{
		this->zeros_map[index_to_change_zeros]++;
	}
	else if ( (this->zeros_map[index_to_change_zeros+1]-1 == letter) )
	{
		this->zeros_map[index_to_change_zeros+1]--;
	}
	else
	{
		character_t *new_zeros;
		new_zeros = new character_t[(this->zeros_map_half_size*2)+2];

		for (character_t i = 0; i < index_to_change_zeros+1; i++)
			new_zeros[i] = this->zeros_map[i];

		new_zeros[index_to_change_zeros+1] = letter;
		new_zeros[index_to_change_zeros+2] = letter;

		for (character_t i = index_to_change_zeros+1; i < (this->zeros_map_half_size*2); i++)
			new_zeros[i+2] = this->zeros_map[i];

		delete[] this->zeros_map;
		this->zeros_map = new_zeros;
		this->zeros_map_half_size += 1;
	}

	return true;
}

template <class character_t>
void TrieNode<character_t>::save_subtrie( std::vector<character_t> current_word, std::vector<character_t> letter_to_append, FILE* file)
{
	// append letter of path to current word
	current_word.insert(current_word.end(), letter_to_append.begin(), letter_to_append.end());

	// write current translation in dictionary file, if there exists one
	if ( this->translation != NULL)
	{
		uint8_t word_size = current_word.size();
		fwrite( &word_size, sizeof(uint8_t), 1, file);
		fwrite( current_word.data(), sizeof(character_t), current_word.size(), file);

		uint16_t translation_size = strlen(this->translation);
		fwrite( &translation_size, sizeof(uint16_t), 1, file);
		fwrite( this->translation, sizeof(character_t), strlen(this->translation), file);
	}

	// read zeros map
	// for every active letter that you find, call recursive saving function
	// we also need the value of the letter, so we can't just get the children_count (like in the destructor)
	character_t next_child = 0;
	character_t_parent current_letter = 0;
	character_t_parent current_zeros_map_position = 0;
	while(current_zeros_map_position != this->zeros_map_half_size*2)
	{
		while (zeros_map[current_zeros_map_position] != current_letter)
			this->children[next_child++]->save_subtrie( current_word, std::vector<character_t>( 1, current_letter++), file);

		current_letter = zeros_map[current_zeros_map_position+1] + 1;
		current_zeros_map_position += 2;
	}

	// letters after last zeros_group (if there exist any)
	character_t_parent alphabet_size =  std::numeric_limits<character_t>::max() + 1;
	while (current_letter != alphabet_size)
		this->children[next_child++]->save_subtrie( current_word, std::vector<character_t>( 1, (character_t) current_letter++), file);
}

template <class character_t>
bool TrieNode<character_t>::get_prefix_words( character_t** toReturn, std::vector<character_t> current_word, std::vector<character_t> letter_to_append, uint8_t& count)
{
	// append letter of path to current word
	current_word.insert(current_word.end(), letter_to_append.begin(), letter_to_append.end());

	// write current word in return list, if there exists one
	if ( this->translation != NULL)
	{
		current_word.push_back( ::trie::end_of_string );
		toReturn[count] = new character_t[current_word.size()];
		strcpy( toReturn[count], &current_word[0]);
		current_word.pop_back();

		if (count == 0)
			return true;
		else
			count--;
	}

	// read zeros map
	// for every active letter that you find, call recursive get_prefix_words function, until count reaches 0
	// we also need the value of the letter, so we can't just get the children_count (like in the destructor)
	character_t next_child = 0;
	character_t_parent current_letter = 0;
	character_t_parent current_zeros_map_position = 0;
	while(current_zeros_map_position != this->zeros_map_half_size*2)
	{
		while (zeros_map[current_zeros_map_position] != current_letter)
			if( this->children[next_child++]->get_prefix_words( toReturn, current_word, std::vector<character_t>( 1, current_letter++), count) )
				return true;

		current_letter = zeros_map[current_zeros_map_position+1] + 1;
		current_zeros_map_position += 2;
	}

	// letters after last zeros_group (if there exist any)
	character_t_parent alphabet_size =  std::numeric_limits<character_t>::max() + 1;
	while (current_letter != alphabet_size)
		if ( this->children[next_child++]->get_prefix_words( toReturn, current_word, std::vector<character_t>( 1, (character_t) current_letter++), count) )
			return true;

	return false;
}

}

#endif