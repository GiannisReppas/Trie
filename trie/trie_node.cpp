#include <cstddef>

#include "def.hpp"
#include "trie_node.hpp"

namespace triectionary
{

TrieNode::TrieNode()
{
	for (uint32_t i = 0; i < (TOTAL_ALPHABET_SIZE/32); i++)
		this->active_letters[i] = 0;

	this->children = NULL;

	this->translation = NULL;
}

TrieNode::~TrieNode()
{
}

uint32_t TrieNode::get_actives_count()
{
	uint32_t toReturn = 0;
	uint32_t c;

	// read array of uin32_t of the node, report number of 1 bits
	for ( uint32_t i = 0; i < (TOTAL_ALPHABET_SIZE/32); i++)
	{
		c = this->active_letters[i];

		// we always assume that 1 uint32_t contains 32 bits, so we do a small loop-unrolling
		if (c & 0b10000000000000000000000000000000)
			++toReturn;
		if (c & 0b01000000000000000000000000000000)
			++toReturn;
		if (c & 0b00100000000000000000000000000000)
			++toReturn;
		if (c & 0b00010000000000000000000000000000)
			++toReturn;
		if (c & 0b00001000000000000000000000000000)
			++toReturn;
		if (c & 0b00000100000000000000000000000000)
			++toReturn;
		if (c & 0b00000010000000000000000000000000)
			++toReturn;
		if (c & 0b00000001000000000000000000000000)
			++toReturn;
		if (c & 0b00000000100000000000000000000000)
			++toReturn;
		if (c & 0b00000000010000000000000000000000)
			++toReturn;
		if (c & 0b00000000001000000000000000000000)
			++toReturn;
		if (c & 0b00000000000100000000000000000000)
			++toReturn;
		if (c & 0b00000000000010000000000000000000)
			++toReturn;
		if (c & 0b00000000000001000000000000000000)
			++toReturn;
		if (c & 0b00000000000000100000000000000000)
			++toReturn;
		if (c & 0b00000000000000010000000000000000)
			++toReturn;
		if (c & 0b00000000000000001000000000000000)
			++toReturn;
		if (c & 0b00000000000000000100000000000000)
			++toReturn;
		if (c & 0b00000000000000000010000000000000)
			++toReturn;
		if (c & 0b00000000000000000001000000000000)
			++toReturn;
		if (c & 0b00000000000000000000100000000000)
			++toReturn;
		if (c & 0b00000000000000000000010000000000)
			++toReturn;
		if (c & 0b00000000000000000000001000000000)
			++toReturn;
		if (c & 0b00000000000000000000000100000000)
			++toReturn;
		if (c & 0b00000000000000000000000010000000)
			++toReturn;
		if (c & 0b00000000000000000000000001000000)
			++toReturn;
		if (c & 0b00000000000000000000000000100000)
			++toReturn;
		if (c & 0b00000000000000000000000000010000)
			++toReturn;
		if (c & 0b00000000000000000000000000001000)
			++toReturn;
		if (c & 0b00000000000000000000000000000100)
			++toReturn;
		if (c & 0b00000000000000000000000000000010)
			++toReturn;
		if (c & 0b00000000000000000000000000000001)
			++toReturn;
	}

	return toReturn;
}

TrieNode* TrieNode::get_node_if_possible( character_t letter )
{
	uint32_t array_element = letter / 32; // unsigned integer index that contains letter's bit
	uint32_t element_bit = letter % 32; // bit index in the unsigned integer with letter info

	// first check if a pointer to a TrieNode exists (otherwise there's no need to check)
	if ((this->active_letters[array_element] << element_bit) & 0b10000000000000000000000000000000 )
	{
		uint32_t pointer_index = 0;
		uint32_t c;

		// count active letters for unit32_t before the uint32_t that contains the letter given as argument
		for ( uint32_t i = 0; i < array_element; i++)
		{
			c = this->active_letters[i];

			// we always assume that 1 uint32_t contains 32 bits, so we do a small loop-unrolling
			if (c & 0b10000000000000000000000000000000)
				++pointer_index;
			if (c & 0b01000000000000000000000000000000)
				++pointer_index;
			if (c & 0b00100000000000000000000000000000)
				++pointer_index;
			if (c & 0b00010000000000000000000000000000)
				++pointer_index;
			if (c & 0b00001000000000000000000000000000)
				++pointer_index;
			if (c & 0b00000100000000000000000000000000)
				++pointer_index;
			if (c & 0b00000010000000000000000000000000)
				++pointer_index;
			if (c & 0b00000001000000000000000000000000)
				++pointer_index;
			if (c & 0b00000000100000000000000000000000)
				++pointer_index;
			if (c & 0b00000000010000000000000000000000)
				++pointer_index;
			if (c & 0b00000000001000000000000000000000)
				++pointer_index;
			if (c & 0b00000000000100000000000000000000)
				++pointer_index;
			if (c & 0b00000000000010000000000000000000)
				++pointer_index;
			if (c & 0b00000000000001000000000000000000)
				++pointer_index;
			if (c & 0b00000000000000100000000000000000)
				++pointer_index;
			if (c & 0b00000000000000010000000000000000)
				++pointer_index;
			if (c & 0b00000000000000001000000000000000)
				++pointer_index;
			if (c & 0b00000000000000000100000000000000)
				++pointer_index;
			if (c & 0b00000000000000000010000000000000)
				++pointer_index;
			if (c & 0b00000000000000000001000000000000)
				++pointer_index;
			if (c & 0b00000000000000000000100000000000)
				++pointer_index;
			if (c & 0b00000000000000000000010000000000)
				++pointer_index;
			if (c & 0b00000000000000000000001000000000)
				++pointer_index;
			if (c & 0b00000000000000000000000100000000)
				++pointer_index;
			if (c & 0b00000000000000000000000010000000)
				++pointer_index;
			if (c & 0b00000000000000000000000001000000)
				++pointer_index;
			if (c & 0b00000000000000000000000000100000)
				++pointer_index;
			if (c & 0b00000000000000000000000000010000)
				++pointer_index;
			if (c & 0b00000000000000000000000000001000)
				++pointer_index;
			if (c & 0b00000000000000000000000000000100)
				++pointer_index;
			if (c & 0b00000000000000000000000000000010)
				++pointer_index;
			if (c & 0b00000000000000000000000000000001)
				++pointer_index;
		}

		// count active letters in the uint32_t that contains the letter given as argument
		// 	until the bit corresponding to the the letter given as argument
		c = this->active_letters[array_element];
		for ( uint32_t i = 0; i < element_bit; i++)
		{
			if (c & 0b10000000000000000000000000000000)
				++pointer_index;
			c  = c << 1;
		}

		return this->children[ pointer_index ];
	}

	return NULL;
}

TrieNode* TrieNode::insert_letter( character_t letter )
{
	uint32_t array_element = letter / 32; // unsigned integer index that contains letter's bit
	uint32_t element_bit = letter % 32; // bit index in the unsigned integer with letter info

	/* 1) First, count number of children pointers and
			find the position at which the new pointer will be inserted */

	// first, create the new TrieNode to return
	TrieNode* toReturn = new TrieNode();

	uint32_t children_count = 0;
	uint32_t c;

	// count active letters for unit32_t before the uint32_t that contains the letter given as argument
	for ( uint32_t i = 0; i < array_element; i++)
	{
		c = this->active_letters[i];

		// we always assume that 1 uint32_t contains 32 bits, so we do a small loop-unrolling
		if (c & 0b10000000000000000000000000000000)
			++children_count;
		if (c & 0b01000000000000000000000000000000)
			++children_count;
		if (c & 0b00100000000000000000000000000000)
			++children_count;
		if (c & 0b00010000000000000000000000000000)
			++children_count;
		if (c & 0b00001000000000000000000000000000)
			++children_count;
		if (c & 0b00000100000000000000000000000000)
			++children_count;
		if (c & 0b00000010000000000000000000000000)
			++children_count;
		if (c & 0b00000001000000000000000000000000)
			++children_count;
		if (c & 0b00000000100000000000000000000000)
			++children_count;
		if (c & 0b00000000010000000000000000000000)
			++children_count;
		if (c & 0b00000000001000000000000000000000)
			++children_count;
		if (c & 0b00000000000100000000000000000000)
			++children_count;
		if (c & 0b00000000000010000000000000000000)
			++children_count;
		if (c & 0b00000000000001000000000000000000)
			++children_count;
		if (c & 0b00000000000000100000000000000000)
			++children_count;
		if (c & 0b00000000000000010000000000000000)
			++children_count;
		if (c & 0b00000000000000001000000000000000)
			++children_count;
		if (c & 0b00000000000000000100000000000000)
			++children_count;
		if (c & 0b00000000000000000010000000000000)
			++children_count;
		if (c & 0b00000000000000000001000000000000)
			++children_count;
		if (c & 0b00000000000000000000100000000000)
			++children_count;
		if (c & 0b00000000000000000000010000000000)
			++children_count;
		if (c & 0b00000000000000000000001000000000)
			++children_count;
		if (c & 0b00000000000000000000000100000000)
			++children_count;
		if (c & 0b00000000000000000000000010000000)
			++children_count;
		if (c & 0b00000000000000000000000001000000)
			++children_count;
		if (c & 0b00000000000000000000000000100000)
			++children_count;
		if (c & 0b00000000000000000000000000010000)
			++children_count;
		if (c & 0b00000000000000000000000000001000)
			++children_count;
		if (c & 0b00000000000000000000000000000100)
			++children_count;
		if (c & 0b00000000000000000000000000000010)
			++children_count;
		if (c & 0b00000000000000000000000000000001)
			++children_count;
	}

	// count active letters in the uint32_t that contains the letter given as argument
	// 	until the bit corresponding to the the letter given as argument
	c = this->active_letters[array_element];
	for ( uint32_t i = 0; i < element_bit; i++)
	{
		if (c & 0b10000000000000000000000000000000)
			++children_count;
		c  = c << 1;
	}

	// we will write at the following index and shift the rest of the array to the right
	uint32_t index_to_insert = children_count;

	// count active letters in the uint32_t that contains the letter given as argument
	// 	after the bit corresponding to the the letter given as argument
	for ( uint32_t i = element_bit; i < 32; i++)
	{
		if (c & 0b10000000000000000000000000000000)
			++children_count;
		c  = c << 1;
	}

	// count active letters after the uint32_t that contains the letter given as argument
	for ( uint32_t i = array_element+1; i < (TOTAL_ALPHABET_SIZE/32); i++)
	{
		c = this->active_letters[i];

		// we always assume that 1 uint32_t contains 8 bits, so we do a small loop-unrolling
		if (c & 0b10000000000000000000000000000000)
			++children_count;
		if (c & 0b01000000000000000000000000000000)
			++children_count;
		if (c & 0b00100000000000000000000000000000)
			++children_count;
		if (c & 0b00010000000000000000000000000000)
			++children_count;
		if (c & 0b00001000000000000000000000000000)
			++children_count;
		if (c & 0b00000100000000000000000000000000)
			++children_count;
		if (c & 0b00000010000000000000000000000000)
			++children_count;
		if (c & 0b00000001000000000000000000000000)
			++children_count;
		if (c & 0b00000000100000000000000000000000)
			++children_count;
		if (c & 0b00000000010000000000000000000000)
			++children_count;
		if (c & 0b00000000001000000000000000000000)
			++children_count;
		if (c & 0b00000000000100000000000000000000)
			++children_count;
		if (c & 0b00000000000010000000000000000000)
			++children_count;
		if (c & 0b00000000000001000000000000000000)
			++children_count;
		if (c & 0b00000000000000100000000000000000)
			++children_count;
		if (c & 0b00000000000000010000000000000000)
			++children_count;
		if (c & 0b00000000000000001000000000000000)
			++children_count;
		if (c & 0b00000000000000000100000000000000)
			++children_count;
		if (c & 0b00000000000000000010000000000000)
			++children_count;
		if (c & 0b00000000000000000001000000000000)
			++children_count;
		if (c & 0b00000000000000000000100000000000)
			++children_count;
		if (c & 0b00000000000000000000010000000000)
			++children_count;
		if (c & 0b00000000000000000000001000000000)
			++children_count;
		if (c & 0b00000000000000000000000100000000)
			++children_count;
		if (c & 0b00000000000000000000000010000000)
			++children_count;
		if (c & 0b00000000000000000000000001000000)
			++children_count;
		if (c & 0b00000000000000000000000000100000)
			++children_count;
		if (c & 0b00000000000000000000000000010000)
			++children_count;
		if (c & 0b00000000000000000000000000001000)
			++children_count;
		if (c & 0b00000000000000000000000000000100)
			++children_count;
		if (c & 0b00000000000000000000000000000010)
			++children_count;
		if (c & 0b00000000000000000000000000000001)
			++children_count;
	}

	/* 2) Now, create a new pointers array of size current_size+1,
			with the extra addition of the pointer for the letter received as argument */

	TrieNode **temp = new TrieNode *[children_count+1];

	for ( uint32_t i = 0; i < index_to_insert; i++)
		temp[i] = this->children[i];

	temp[index_to_insert] = toReturn;

	for ( uint32_t i = index_to_insert+1; i < children_count+1; i++)
		temp[i] = this->children[i-1];

	delete[] this->children;
	this->children = temp;

	/* 3) lastly, set the corresponding bit in the active letters to 1 */

	this->active_letters[array_element] =
		this->active_letters[array_element] | (0b10000000000000000000000000000000 >> element_bit);

	return toReturn;
}

void TrieNode::set_child_null( character_t letter )
{
	uint32_t array_element = letter / 32; // unsigned integer index that contains letter's bit
	uint32_t element_bit = letter % 32; // bit index in the unsigned integer with letter info

	/* 1) First, count number of children pointers and
			find the index of the pointer to be deleted */

	uint32_t children_count = 0;
	uint32_t c;

	// count active letters for unit32_t before the uint32_t that contains the letter given as argument
	for ( uint32_t i = 0; i < array_element; i++)
	{
		c = this->active_letters[i];

		// we always assume that 1 uint32_t contains 32 bits, so we do a small loop-unrolling
		if (c & 0b10000000000000000000000000000000)
			++children_count;
		if (c & 0b01000000000000000000000000000000)
			++children_count;
		if (c & 0b00100000000000000000000000000000)
			++children_count;
		if (c & 0b00010000000000000000000000000000)
			++children_count;
		if (c & 0b00001000000000000000000000000000)
			++children_count;
		if (c & 0b00000100000000000000000000000000)
			++children_count;
		if (c & 0b00000010000000000000000000000000)
			++children_count;
		if (c & 0b00000001000000000000000000000000)
			++children_count;
		if (c & 0b00000000100000000000000000000000)
			++children_count;
		if (c & 0b00000000010000000000000000000000)
			++children_count;
		if (c & 0b00000000001000000000000000000000)
			++children_count;
		if (c & 0b00000000000100000000000000000000)
			++children_count;
		if (c & 0b00000000000010000000000000000000)
			++children_count;
		if (c & 0b00000000000001000000000000000000)
			++children_count;
		if (c & 0b00000000000000100000000000000000)
			++children_count;
		if (c & 0b00000000000000010000000000000000)
			++children_count;
		if (c & 0b00000000000000001000000000000000)
			++children_count;
		if (c & 0b00000000000000000100000000000000)
			++children_count;
		if (c & 0b00000000000000000010000000000000)
			++children_count;
		if (c & 0b00000000000000000001000000000000)
			++children_count;
		if (c & 0b00000000000000000000100000000000)
			++children_count;
		if (c & 0b00000000000000000000010000000000)
			++children_count;
		if (c & 0b00000000000000000000001000000000)
			++children_count;
		if (c & 0b00000000000000000000000100000000)
			++children_count;
		if (c & 0b00000000000000000000000010000000)
			++children_count;
		if (c & 0b00000000000000000000000001000000)
			++children_count;
		if (c & 0b00000000000000000000000000100000)
			++children_count;
		if (c & 0b00000000000000000000000000010000)
			++children_count;
		if (c & 0b00000000000000000000000000001000)
			++children_count;
		if (c & 0b00000000000000000000000000000100)
			++children_count;
		if (c & 0b00000000000000000000000000000010)
			++children_count;
		if (c & 0b00000000000000000000000000000001)
			++children_count;
	}

	// count active letters in the uint32_t that contains the letter given as argument
	// 	until the bit corresponding to the the letter given as argument
	c = this->active_letters[array_element];
	for ( uint32_t i = 0; i < element_bit; i++)
	{
		if (c & 0b10000000000000000000000000000000)
			++children_count;
		c  = c << 1;
	}

	// we will delete the pointer at the following index and shift the rest of the array to the left
	uint32_t index_to_delete = children_count;

	// count active letters in the uint32_t that contains the letter given as argument
	// 	after the bit corresponding to the the letter given as argument
	for ( uint32_t i = element_bit; i < 32; i++)
	{
		if (c & 0b10000000000000000000000000000000)
			++children_count;
		c  = c << 1;
	}

	// count active letters after the uint32_t that contains the letter given as argument
	for ( uint32_t i = array_element+1; i < (TOTAL_ALPHABET_SIZE/32); i++)
	{
		c = this->active_letters[i];

		// we always assume that 1 uint32_t contains 8 bits, so we do a small loop-unrolling
		if (c & 0b10000000000000000000000000000000)
			++children_count;
		if (c & 0b01000000000000000000000000000000)
			++children_count;
		if (c & 0b00100000000000000000000000000000)
			++children_count;
		if (c & 0b00010000000000000000000000000000)
			++children_count;
		if (c & 0b00001000000000000000000000000000)
			++children_count;
		if (c & 0b00000100000000000000000000000000)
			++children_count;
		if (c & 0b00000010000000000000000000000000)
			++children_count;
		if (c & 0b00000001000000000000000000000000)
			++children_count;
		if (c & 0b00000000100000000000000000000000)
			++children_count;
		if (c & 0b00000000010000000000000000000000)
			++children_count;
		if (c & 0b00000000001000000000000000000000)
			++children_count;
		if (c & 0b00000000000100000000000000000000)
			++children_count;
		if (c & 0b00000000000010000000000000000000)
			++children_count;
		if (c & 0b00000000000001000000000000000000)
			++children_count;
		if (c & 0b00000000000000100000000000000000)
			++children_count;
		if (c & 0b00000000000000010000000000000000)
			++children_count;
		if (c & 0b00000000000000001000000000000000)
			++children_count;
		if (c & 0b00000000000000000100000000000000)
			++children_count;
		if (c & 0b00000000000000000010000000000000)
			++children_count;
		if (c & 0b00000000000000000001000000000000)
			++children_count;
		if (c & 0b00000000000000000000100000000000)
			++children_count;
		if (c & 0b00000000000000000000010000000000)
			++children_count;
		if (c & 0b00000000000000000000001000000000)
			++children_count;
		if (c & 0b00000000000000000000000100000000)
			++children_count;
		if (c & 0b00000000000000000000000010000000)
			++children_count;
		if (c & 0b00000000000000000000000001000000)
			++children_count;
		if (c & 0b00000000000000000000000000100000)
			++children_count;
		if (c & 0b00000000000000000000000000010000)
			++children_count;
		if (c & 0b00000000000000000000000000001000)
			++children_count;
		if (c & 0b00000000000000000000000000000100)
			++children_count;
		if (c & 0b00000000000000000000000000000010)
			++children_count;
		if (c & 0b00000000000000000000000000000001)
			++children_count;
	}

	/* 2) Now, create a new pointers array of size current_size-1,
			without the pointer for the letter received as argument */

	TrieNode **temp;
	(children_count != 1) ? temp = new TrieNode *[children_count-1] : temp = NULL;

	for ( uint32_t i = 0; i < index_to_delete; i++)
		temp[i] = this->children[i];

	for ( uint32_t i = index_to_delete+1; i < children_count; i++)
		temp[i-1] = this->children[i];

	delete[] this->children;
	this->children = temp;

	/* 3) Lastly, set the corresponding bit in the active letters to 0 */

	this->active_letters[array_element] =
		this->active_letters[array_element] & ~(0b10000000000000000000000000000000 >> element_bit);
}

}