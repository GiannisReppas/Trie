#ifndef TRIE_FUNCTIONS_H_
#define TRIE_FUNCTIONS_H_

#include <string>
#include <vector>

namespace trie
{
/* compares two series, until the first end_of_string */
template <typename character_t>
int strcmp(const character_t* s1, const character_t* s2, character_t end_of_string = 0)
{
	character_t current_index = 0;

	while (s1[current_index] == s2[current_index])
	{
		if ( (s1 == end_of_string) || (s2 == end_of_string) )
			break;

		current_index++;
	}

	if (s1[current_index] == s2[current_index])
		return 0;
	else if (s1[current_index] > s2[current_index])
		return 1;
	else if (s1[current_index] < s2[current_index])
		return -1;
}

/* reports number of elements in the array, until the first end_of_string */
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
template <typename character_t>
uint32_t strlen(const std::vector<character_t> array, character_t end_of_string = 0)
{
	return strlen( array.data(), end_of_string );
}

/* copies elements from second to first (!preallocated!) array, until the first end_of_string */
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