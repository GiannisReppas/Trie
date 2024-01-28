#ifndef TRIE_TRIE_NODE_H_
#define TRIE_TRIE_NODE_H_

#include <stdint.h>
#include <limits>

#include "trie/extended_character_functions.hpp"

namespace trie
{

template <class character_t> 
class TrieNode
{
private:
	/* variable size ( 2 to ((ALPHABET_SIZE/2) * 2) * sizeof(character_t) )
		worst case when bits have the form of 01010101... zeros_map add 2 elements for
		each 0 bit
		due to this case, zeros_map may have size larger than max. value of character_t
		e.g. size of 256 for character_t -> uint8_t
		as a result, we keep half of zeros_map size, for every pair that it keeps
		that way, we save memory */
	character_t *zeros_map;
	character_t zeros_map_half_size;

	/* variable size (0 to ALPHABET_SIZE*sizeof(pointer)) bytes
	 pointer usually 8 bytes */
	class TrieNode **children;

	/* variable size, 0 to translation_size*sizeof(character_size)+sizeof(character_size) bytes
	 uint8_t  always 1 byte
	 uint16_t always 2 bytes
	 char32_t always 4 bytes */
	character_t *translation;

public:
	TrieNode();
	~TrieNode();

	bool is_empty();

	/* getters */
	uint32_t get_children_count(); /* returns size of array children */
	character_t* get_translation();
	character_t get_zeros_map_half_size();
	character_t* get_zeros_map();
	TrieNode** get_children();

	void set_translation(const character_t* translation);

	/* returns a Trienode pointer following the path of the argument letter, if there exists one */
	TrieNode* get_node_if_possible( character_t letter );
	/* adds a new Trienode path in current Trienode, updates both zeros_map and children
		assumes that letter given as argument will always be 0 in current zeros_map */
	TrieNode* insert_letter( character_t letter );
	/* deletes a Trienode path in current Trienode, updates both zeros_map and children
		assumes that letter given as argument will always be 1 in current zeros_map */
	void set_child_null( character_t letter );
};

template <class character_t> 
TrieNode<character_t>::TrieNode()
{
	// 2 elements to depict a full zero group for all bits
	this->zeros_map = new character_t[2];
	this->zeros_map[0] = 0;
	this->zeros_map[1] = std::numeric_limits<character_t>::max();
	this->zeros_map_half_size = 1;

	// no children in the node
	this->children = NULL;

	// no translation in the node
	this->translation = NULL;
}

template <class character_t> 
TrieNode<character_t>::~TrieNode()
{
}

template <class character_t> 
uint32_t TrieNode<character_t>::get_children_count()
{
	uint32_t children_count = 0;

	// ones before the 1st zeros group
	children_count += this->zeros_map[0];

	// ones between the zeros groups
	for ( uint32_t i = 0; i < (this->zeros_map_half_size*2)-2; i += 2)
		children_count += this->zeros_map[i+2] - this->zeros_map[i+1] - 1;

	// ones after the last zeros group
	children_count += std::numeric_limits<character_t>::max() - this->zeros_map[(this->zeros_map_half_size*2)-1];

	return children_count;
}

template <class character_t>
bool TrieNode<character_t>::is_empty()
{
	return (this->children == NULL);
	// reutrn (this->get_children_count == 0);
}

template <class character_t>
character_t* TrieNode<character_t>::get_translation()
{
	return this->translation;
}

template <class character_t>
character_t TrieNode<character_t>::get_zeros_map_half_size()
{
	return this->zeros_map_half_size*2;
}

template <class character_t>
character_t* TrieNode<character_t>::get_zeros_map()
{
	return this->zeros_map;
}

template <class character_t>
TrieNode<character_t>** TrieNode<character_t>::get_children()
{
	return this->children;
}

template <class character_t>
void TrieNode<character_t>::set_translation(const character_t* translation)
{
	if (this->translation != NULL)
		delete[] this->translation;

	if (translation != NULL)
	{
		this->translation = new character_t[strlen(translation) + 1];
		strcpy( this->translation, translation);
	}
	else
		translation = NULL;
}

/* returns a Trienode pointer following the path of the argument letter, if there exists one */
template <class character_t>
TrieNode<character_t>* TrieNode<character_t>::get_node_if_possible( character_t letter )
{
	uint32_t children_count = 0;

	// letter exists before the 1st zeros group (ones group)
	if (letter < this->zeros_map[0])
		return this->children[ letter ];

	// update current count
	children_count += this->zeros_map[0];

	for ( uint32_t i = 0; i < (this->zeros_map_half_size*2)-2; i += 2)
	{
		// letter exists in a zeros group
		if ( (this->zeros_map[i] <= letter) && (letter <= this->zeros_map[i+1]) )
			return NULL;

		// letter exists between 2 zeros groups (ones group)
		if ( (this->zeros_map[i+1] < letter) && (letter < this->zeros_map[i+2]) )
			return this->children[ children_count + (letter - this->zeros_map[i+1]) - 1 ];

		// update current count
		children_count += this->zeros_map[i+2] - this->zeros_map[i+1] - 1;
	}

	// letter exists in the last zeros group
	uint32_t i = (this->zeros_map_half_size*2) - 2;
	if ( (this->zeros_map[i] <= letter) && (letter <= this->zeros_map[i+1]) )
		return NULL;

	// letter exists after the last zeros group (ones group)
	if (this->zeros_map[i+1] < letter)
		return this->children[ children_count + (letter - this->zeros_map[i+1]) - 1 ];

	printf("Error while searching trie_node\n");

	return NULL;
}

/* adds a new Trienode path in current Trienode, updates both zeros_map and children
	assumes that letter given as argument will always be 0 in current zeros_map */
template <class character_t>
TrieNode<character_t>* TrieNode<character_t>::insert_letter( character_t letter )
{
	/* 1) First, count number of children pointers, find the index of zeros_map that will be changed
			and the position at which the new pointer will be inserted in childrean array */

	uint32_t children_count = 0;
	uint32_t index_to_insert_children, index_to_change_zeros;

	// update current count
	children_count += this->zeros_map[0];

	for ( uint32_t i = 0; i < (this->zeros_map_half_size*2)-2; i += 2)
	{
		// found the zero group that letter exists
		if ( (this->zeros_map[i] <= letter) && (letter <= this->zeros_map[i+1]) )
		{
			index_to_change_zeros = i;
			index_to_insert_children = children_count;
		}

		// update current count
		children_count += this->zeros_map[i+2] - this->zeros_map[i+1] - 1;
	}

	// letter exists in the last zeros group
	if ( (this->zeros_map[(this->zeros_map_half_size*2)-2] <= letter) && (letter <= this->zeros_map[(this->zeros_map_half_size*2)-1]) )
	{
		index_to_change_zeros = (this->zeros_map_half_size*2)-2;
		index_to_insert_children = children_count;
	}

	// update current count
	children_count += std::numeric_limits<character_t>::max() - this->zeros_map[(this->zeros_map_half_size*2)-1];


	/* 2) Now, create a new pointers array of size current_size+1,
			with the extra addition of the pointer for the letter received as argument */

	// Create the new TrieNode to return
	TrieNode* toReturn = new TrieNode<character_t>();

	// Create new child pointers array
	TrieNode **new_children = new TrieNode<character_t> *[children_count+1];

	for ( uint32_t i = 0; i < index_to_insert_children; i++)
		new_children[i] = this->children[i];

	new_children[index_to_insert_children] = toReturn;

	for ( uint32_t i = index_to_insert_children+1; i < children_count+1; i++)
		new_children[i] = this->children[i-1];

	// swap with current child pointers array and delete the old one
	delete[] this->children;
	this->children = new_children;


	/* 3) Lastly, update the zeros map array
		there are 3 different cases : 1) e.g. for [8,13], make 10 one -> [8,9] and [11,13] (insert 2)
										  OR  for [8,10], make 9  one -> [8,8] and [10,10] (insert 2)
									 2a) e.g. for [8,9],  make 8  one -> [9,9] (just change 1)
									 2b) e.g. for [8,9],  make 9  one -> [8,8] (just change 1)
									  3) e.g. for [8,8],  make 8  one -> - (delete 2) */

	if ( (this->zeros_map[index_to_change_zeros] < letter) && (letter < this->zeros_map[index_to_change_zeros+1]) )
	{
		character_t *new_zeros;
		uint32_t old_end_value = this->zeros_map[index_to_change_zeros+1];
		this->zeros_map[index_to_change_zeros+1] = letter-1;
		new_zeros = new character_t[(this->zeros_map_half_size*2)+2];

		for (uint32_t i = 0; i < index_to_change_zeros+2; i++)
			new_zeros[i] = this->zeros_map[i];

		new_zeros[index_to_change_zeros+2] = letter+1;
		new_zeros[index_to_change_zeros+3] = old_end_value;

		for (uint32_t i = index_to_change_zeros+4; i < (this->zeros_map_half_size*2)+2; i ++)
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

		for (uint32_t i = 0; i < index_to_change_zeros; i++)
			new_zeros[i] = this->zeros_map[i];

		for (uint32_t i = index_to_change_zeros+2; i < (this->zeros_map_half_size*2); i++)
			new_zeros[i-2] = this->zeros_map[i];

		delete[] this->zeros_map;
		this->zeros_map = new_zeros;
		this->zeros_map_half_size -= 1;
	}


	return toReturn;
}
	
/* deletes a Trienode path in current Trienode, updates both zeros_map and children
	assumes that letter given as argument will always be 1 in current zeros_map */
template <class character_t>
void TrieNode<character_t>::set_child_null( character_t letter )
{
	/* 1) First, count number of children pointers, find the index of zeros_map that will be changed
			and the position at which the new pointer will be inserted in childrean array */

	uint32_t children_count = 0;
	uint32_t index_to_delete_children, index_to_change_zeros;

	// letter exists before the 1st zeros group (ones group)
	// in this case, we don't set index_to_change_zeros, because it can't have a value of -1
	if ( letter < this->zeros_map[0])
	{
		index_to_delete_children = letter;
	}

	// update current count
	children_count += this->zeros_map[0];

	for ( uint32_t i = 0; i < (this->zeros_map_half_size*2)-2; i += 2)
	{
		// found the zero group after which the letter exists
		if ( (this->zeros_map[i+1] < letter) && (letter < this->zeros_map[i+2]) )
		{
			index_to_change_zeros = i+1;
			index_to_delete_children = children_count + (letter - this->zeros_map[i+1]) - 1;
		}

		// update current count
		children_count += this->zeros_map[i+2] - this->zeros_map[i+1] - 1;
	}

	// letter exists after the last zeros group
	if ( (this->zeros_map[(this->zeros_map_half_size*2)-1] < letter) )
	{
		index_to_change_zeros = (this->zeros_map_half_size*2)-1;
		index_to_delete_children = children_count + (letter - this->zeros_map[(this->zeros_map_half_size*2) - 1]) - 1;
	}

	// update current count
	children_count += std::numeric_limits<character_t>::max() - this->zeros_map[(this->zeros_map_half_size*2)-1];
		


	/* 2) Now, create a new pointers array of size current_size-1,
			without the pointer for the letter received as argument */
	// Create new child pointers array
	TrieNode **temp;
	(children_count > 1) ? temp = new TrieNode<character_t> *[children_count-1] : temp = NULL;

	for ( uint32_t i = 0; i < index_to_delete_children; i++)
		temp[i] = this->children[i];

	for ( uint32_t i = index_to_delete_children+1; i < children_count; i++)
		temp[i-1] = this->children[i];

	// swap with current child pointers array and delete the old one
	delete[] this->children;
	this->children = temp;

	/* 3) Lastly, update the zeros map array
		there are 5 different cases : 1) edge-case, 1 exists before zeros_map[0]
											if (letter == zeros_map[0]-1)
												e.g.  [5,x], make 4 zero  -> [4,x] (just change 1)
											else
												e.g.  [5,x], make 3 zero  -> [3,3] [5,x] (insert 2)
									  2) edge-case, 1 exists after zeros_map[(zeros_map_half_size*2)-1]
											if (letter == zeros_map[(zeros_map_half_size*2)-1]+1)
											   e.g.  [x,97], make 98 zero -> [x,98] (just change 1)
											else
											   e.g.  [x,97], make 99 zero -> [x,97] [99,99] (insert 2)
									  3) e.g. [x,60] [62,y], make 61 zero -> [x,y] (delete 2)
									 4a) e.g. [x,60] [70,y], make 61 zero -> [x,61] [70,y] (just change 1)
									 4b) e.g. [x,60] [70,y], make 69 zero -> [x,60] [69,y] (just change 1)
									  5) e.g. [x,60] [70,y], make 62 zero -> [x,61] [62,62] [70,y] (insert 2) */

	// edge cases
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

			for (uint32_t i = 0; i < (this->zeros_map_half_size*2); i++)
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

			for (uint32_t i = 0; i < (this->zeros_map_half_size*2); i++)
				new_zeros[i] = this->zeros_map[i];

			new_zeros[index_to_change_zeros+1] = letter;
			new_zeros[index_to_change_zeros+2] = letter;

			delete[] this->zeros_map;
			this->zeros_map = new_zeros;
			this->zeros_map_half_size += 1;
		}
	}
	// normal cases
	else if ( (this->zeros_map[index_to_change_zeros+1] - this->zeros_map[index_to_change_zeros]) == 2)
	{
		this->zeros_map[index_to_change_zeros] = this->zeros_map[index_to_change_zeros+2];

		character_t *new_zeros;
		new_zeros = new character_t[(this->zeros_map_half_size*2)-2];

		for (uint32_t i = 0; i < index_to_change_zeros+1; i++)
			new_zeros[i] = this->zeros_map[i];

		for (uint32_t i = index_to_change_zeros+3; i < (this->zeros_map_half_size*2); i++)
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

		for (uint32_t i = 0; i < index_to_change_zeros+1; i++)
			new_zeros[i] = this->zeros_map[i];

		new_zeros[index_to_change_zeros+1] = letter;
		new_zeros[index_to_change_zeros+2] = letter;

		for (uint32_t i = index_to_change_zeros+1; i < (this->zeros_map_half_size*2); i++)
			new_zeros[i+2] = this->zeros_map[i];

		delete[] this->zeros_map;
		this->zeros_map = new_zeros;
		this->zeros_map_half_size += 1;
	}
}



}

#endif