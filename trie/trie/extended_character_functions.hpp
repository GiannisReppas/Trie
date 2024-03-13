#ifndef TRIE_FUNCTIONS_H_
#define TRIE_FUNCTIONS_H_

#include <string>

namespace trie
{

/* reports number of elements in the array until the first ::trie::end_of_string
	end_of_string is '\0' by default */
template <typename character_t>
uint32_t strlen(const character_t* array, character_t end_of_string = 0)
{
	uint32_t toReturn = 0;
	character_t current_index = 0;

	while (array[current_index++] != end_of_string)
	{
		++toReturn;
	}

	return toReturn;
}

/* copies elements from second to first array, which is preallocated, until the first end_of_string
	end_of_string is '\0' by default */
template <typename character_t>
character_t* strcpy(character_t* dst, const character_t* src, character_t end_of_string = 0)
{
	character_t current_index = 0;

	while (src[current_index] != end_of_string)
	{
		dst[current_index] = src[current_index];

		++current_index;
	}

	dst[current_index] = end_of_string;

	return dst;
}

}

#endif