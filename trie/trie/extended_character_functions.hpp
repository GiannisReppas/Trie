#ifndef TRIE_FUNCTIONS_H_
#define TRIE_FUNCTIONS_H_

#include <string>

namespace trie
{

/* number that should be counted as the end of a series of integers
	normally, it is 0, like in strings */
uint32_t end_of_string = 0;

/* reports number of elements in the array until the first ::trie::end_of_string */
template <typename character_t>
uint32_t strlen(const character_t* array)
{
	uint32_t toReturn = 0;
	uint32_t current_index = 0;

	while (array[current_index++] != ::trie::end_of_string)
	{
		++toReturn;
	}

	return toReturn;
}

/* copies elements from second to first array, which is preallocated, until the first ::trie::end_of_string */
template <typename character_t>
character_t* strcpy(character_t* dst, const character_t* src)
{
	uint32_t current_index = 0;

	while (src[current_index] != ::trie::end_of_string)
	{
		dst[current_index] = src[current_index];

		++current_index;
	}

	dst[current_index] = ::trie::end_of_string;

	return dst;
}

/* reads an std::string and writes its elements to a character_t*
	be carefull, ::trie::end_of_string is '\0' for std::string */
template <typename character_t>
character_t* str_to_c(std::string toConvert)
{
	uint32_t toConvertSize = toConvert.size();

	character_t* toReturn = new character_t[toConvertSize + 1];

	for (uint32_t i = 0; i < toConvertSize; i++)
	{
		toReturn[i] = toConvert[i];
	}

	toReturn[toConvertSize] = ::trie::end_of_string;

	return toReturn;
}

}

#endif