#include "def.hpp"
#include "extended_character_functions.hpp"

namespace triectionary
{
	uint32_t strlen(const character_t* array)
	{
		uint32_t toReturn = 0;
		uint32_t current_index = 0;

		while (array[current_index++] != END_OF_STRING)
		{
			++toReturn;
		}

		return toReturn;
	}

	character_t* strcpy(character_t* dst, const character_t* src)
	{
		uint32_t current_index = 0;

		while (src[current_index] != END_OF_STRING)
		{
			dst[current_index] = src[current_index];

			++current_index;
		}

		dst[current_index] = END_OF_STRING;

		return dst;
	}

	character_t* str_to_c(std::string toConvert)
	{
		uint32_t toConvertSize = toConvert.size();

		character_t* toReturn = new character_t[toConvertSize + 1];

		for (uint32_t i = 0; i < toConvertSize; i++)
		{
			toReturn[i] = toConvert[i];
		}

		toReturn[toConvertSize] = END_OF_STRING;

		return toReturn;
	}

}