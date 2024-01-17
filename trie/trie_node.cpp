#include <cstddef>

#include "def.hpp"
#include "trie_node.hpp"

namespace triectionary
{

TrieNode::TrieNode()
{
	this->active_letters = new character_t[2];
	this->active_letters[0] = 0;
	this->active_letters[1] = TOTAL_ALPHABET_SIZE - 1;
	this->active_letters_size = 2;

	this->children = NULL;

	this->translation = NULL;
}

TrieNode::~TrieNode()
{
}

uint32_t TrieNode::get_children_count()
{
	uint32_t children_count = 0;

	children_count += this->active_letters[0];

	for ( uint32_t i = 0; i < this->active_letters_size-2; i += 2)
		children_count += this->active_letters[i+2] - this->active_letters[i+1] - 1;

	children_count += (TOTAL_ALPHABET_SIZE-1) - this->active_letters[this->active_letters_size-1];

	return children_count;
}

TrieNode* TrieNode::get_node_if_possible( character_t letter )
{
	if (this->active_letters_size != 2)
	{
		uint32_t children_count = 0;

		// letter is a one
		if (letter < this->active_letters[0])
			return this->children[ letter ];

		// update current count
		children_count += this->active_letters[0];

		for ( uint32_t i = 0; i < this->active_letters_size-2; i += 2)
		{
			// letter is a zero
			if ( (this->active_letters[i] <= letter) && (letter <= this->active_letters[i+1]) )
				return NULL;

			// letter is a one
			if ( (this->active_letters[i+1] < letter) && (letter < this->active_letters[i+2]) )
				return this->children[ children_count + (letter - this->active_letters[i+1]) - 1 ];

			// update current count
			children_count += this->active_letters[i+2] - this->active_letters[i+1] - 1;
		}

		// letter is a zero
		uint32_t i = this->active_letters_size - 2;
		if ( (this->active_letters[i] <= letter) && (letter <= this->active_letters[i+1]) )
			return NULL;

		// letter is a one
		if (this->active_letters[i+1] < letter)
			return this->children[ children_count + (letter - this->active_letters[i+1]) - 1 ];

		printf("Error while searching trie_node\n");
	}

	return NULL;
}

TrieNode* TrieNode::insert_letter( character_t letter )
{
	/* 1) First, count number of children pointers and
			find the position at which the new pointer will be inserted */

	uint32_t children_count = 0;
	uint32_t index_to_insert_children, index_to_insert_zeros;

	children_count += this->active_letters[0];

	for ( uint32_t i = 0; i < this->active_letters_size-2; i += 2)
	{
		// letter is a zero
		if ( (this->active_letters[i] <= letter) && (letter <= this->active_letters[i+1]) )
		{
			index_to_insert_zeros = i;
			index_to_insert_children = children_count;
		}

		// update current count
		children_count += this->active_letters[i+2] - this->active_letters[i+1] - 1;
	}

	// letter is a zero
	if ( (this->active_letters[this->active_letters_size-2] <= letter) && (letter <= this->active_letters[this->active_letters_size-1]) )
	{
		index_to_insert_zeros = this->active_letters_size-2;
		index_to_insert_children = children_count;
	}

	children_count += (TOTAL_ALPHABET_SIZE-1) - this->active_letters[this->active_letters_size-1];


	/* 2) Now, create a new pointers array of size current_size+1,
			with the extra addition of the pointer for the letter received as argument */

	// Create the new TrieNode to return
	TrieNode* toReturn = new TrieNode();

	// Create new child pointers array
	TrieNode **new_children = new TrieNode *[children_count+1];

	for ( uint32_t i = 0; i < index_to_insert_children; i++)
		new_children[i] = this->children[i];

	new_children[index_to_insert_children] = toReturn;

	for ( uint32_t i = index_to_insert_children+1; i < children_count+1; i++)
		new_children[i] = this->children[i-1];

	// swap with current child pointers array and delete it
	delete[] this->children;
	this->children = new_children;


	/* 3) Lastly, update the zeros map array */

	if ( (this->active_letters[index_to_insert_zeros] < letter) && (letter < this->active_letters[index_to_insert_zeros+1]) )
	{
		character_t *new_zeros;
		uint32_t old_end_value = this->active_letters[index_to_insert_zeros+1];
		this->active_letters[index_to_insert_zeros+1] = letter-1;
		new_zeros = new character_t[this->active_letters_size+2];

		for (uint32_t i = 0; i < index_to_insert_zeros+2; i++)
			new_zeros[i] = this->active_letters[i];

		new_zeros[index_to_insert_zeros+2] = letter+1;
		new_zeros[index_to_insert_zeros+3] = old_end_value;

		for (uint32_t i = index_to_insert_zeros+4; i < this->active_letters_size+2; i ++)
			new_zeros[i] = this->active_letters[i-2];

		delete[] this->active_letters;
		this->active_letters = new_zeros;
		this->active_letters_size += 2;
	}
	else if ( (letter == this->active_letters[index_to_insert_zeros]) && (letter != this->active_letters[index_to_insert_zeros+1]) )
	{
		this->active_letters[index_to_insert_zeros] += 1;
	}
	else if ( (letter != this->active_letters[index_to_insert_zeros]) && (letter== this->active_letters[index_to_insert_zeros+1]) )
	{
		this->active_letters[index_to_insert_zeros+1] -= 1;
	}
	else
	{
		character_t *new_zeros;
		new_zeros = new character_t[this->active_letters_size-2];

		for (uint32_t i = 0; i < index_to_insert_zeros; i++)
			new_zeros[i] = this->active_letters[i];

		for (uint32_t i = index_to_insert_zeros+2; i < this->active_letters_size; i++)
			new_zeros[i-2] = this->active_letters[i];

		delete[] this->active_letters;
		this->active_letters = new_zeros;
		this->active_letters_size -= 2;
	}


	return toReturn;
}

void TrieNode::set_child_null( character_t letter )
{
	/* 1) First, count number of children pointers and
			find the index of the pointer to be deleted */

	uint32_t children_count = 0;
	uint32_t index_to_delete_children, index_to_delete_zeros;

	if ( letter < this->active_letters[0])
	{
		index_to_delete_children = letter;
	}

	children_count += this->active_letters[0];

	for ( uint32_t i = 0; i < this->active_letters_size-2; i += 2)
	{
		// letter is a one
		if ( (this->active_letters[i+1] < letter) && (letter < this->active_letters[i+2]) )
		{
			index_to_delete_zeros = i+1;
			index_to_delete_children = children_count + (letter - this->active_letters[i+1]) - 1;
		}

		// update current count
		children_count += this->active_letters[i+2] - this->active_letters[i+1] - 1;
	}

	// letter is a one
	if ( (this->active_letters[this->active_letters_size-1] < letter) )
	{
		index_to_delete_zeros = this->active_letters_size-1;
		index_to_delete_children = children_count + (letter - this->active_letters[this->active_letters_size - 1]) - 1;
	}

	children_count += (TOTAL_ALPHABET_SIZE-1) - this->active_letters[this->active_letters_size-1];


	/* 2) Now, create a new pointers array of size current_size-1,
			without the pointer for the letter received as argument */

	TrieNode **temp;
	(children_count > 1) ? temp = new TrieNode *[children_count-1] : temp = NULL;

	for ( uint32_t i = 0; i < index_to_delete_children; i++)
		temp[i] = this->children[i];

	for ( uint32_t i = index_to_delete_children+1; i < children_count; i++)
		temp[i-1] = this->children[i];

	delete[] this->children;
	this->children = temp;


	/* 3) Lastly, update the zeros map array */

	// edge cases (left-most,right-most)
	if (letter < this->active_letters[0])
	{
		if (letter == this->active_letters[0]-1)
		{
			this->active_letters[0]--;
		}
		else
		{
			character_t *new_zeros;
			new_zeros = new character_t[this->active_letters_size+2];

			new_zeros[0] = letter;
			new_zeros[1] = letter;

			for (uint32_t i = 0; i < this->active_letters_size; i++)
				new_zeros[i+2] = this->active_letters[i];

			delete[] this->active_letters;
			this->active_letters = new_zeros;
			this->active_letters_size += 2;
		}
	}
	else if (index_to_delete_zeros == this->active_letters_size-1)
	{
		if (letter == this->active_letters[index_to_delete_zeros] + 1)
		{
			this->active_letters[index_to_delete_zeros]++;
		}
		else
		{
			character_t *new_zeros;
			new_zeros = new character_t[this->active_letters_size+2];

			for (uint32_t i = 0; i < this->active_letters_size; i++)
				new_zeros[i] = this->active_letters[i];

			new_zeros[index_to_delete_zeros+1] = letter;
			new_zeros[index_to_delete_zeros+2] = letter;

			delete[] this->active_letters;
			this->active_letters = new_zeros;
			this->active_letters_size += 2;
		}
	}
	// normal cases
	else if ((this->active_letters[index_to_delete_zeros+1] - this->active_letters[index_to_delete_zeros]) == 2)
	{
		this->active_letters[index_to_delete_zeros] = this->active_letters[index_to_delete_zeros+2];

		character_t *new_zeros;
		new_zeros = new character_t[this->active_letters_size-2];

		for (uint32_t i = 0; i < index_to_delete_zeros+1; i++)
			new_zeros[i] = this->active_letters[i];

		for (uint32_t i = index_to_delete_zeros+3; i < this->active_letters_size; i++)
			new_zeros[i-2] = this->active_letters[i];

		delete[] this->active_letters;
		this->active_letters = new_zeros;
		this->active_letters_size -= 2;
	}
	else if ( (this->active_letters[index_to_delete_zeros]+1 == letter) )
	{
		this->active_letters[index_to_delete_zeros]++;
	}
	else if ( (this->active_letters[index_to_delete_zeros+1]-1 == letter) )
	{
		this->active_letters[index_to_delete_zeros+1]--;
	}
	else
	{
		character_t *new_zeros;
		new_zeros = new character_t[this->active_letters_size+2];

		for (uint32_t i = 0; i < index_to_delete_zeros+1; i++)
			new_zeros[i] = this->active_letters[i];

		new_zeros[index_to_delete_zeros+1] = letter;
		new_zeros[index_to_delete_zeros+2] = letter;

		for (uint32_t i = index_to_delete_zeros+1; i < this->active_letters_size; i++)
			new_zeros[i+2] = this->active_letters[i];

		delete[] this->active_letters;
		this->active_letters = new_zeros;
		this->active_letters_size += 2;
	}
}

}