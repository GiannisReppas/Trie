#include "def.hpp"
#include "extended_character_functions.hpp"

namespace triectionary
{
	uint32_t strlen(const character_t* array)
	{
		uint32_t toReturn = 0;
		uint32_t current_index = 0;

		while (array[current_index++] != '\0')
		{
			++toReturn;
		}

		return toReturn;
	}

	character_t* strcpy(character_t* dst, const character_t* src)
	{
		uint32_t current_index = 0;

		while (src[current_index] != '\0')
		{
			dst[current_index] = src[current_index];

			++current_index;
		}

		dst[current_index] = '\0';

		return dst;
	}

	character_t* str_to_c(std::string toConvert)
	{
		uint8_t toConvertSize = toConvert.size();

		character_t* toReturn = new character_t[toConvertSize + 1];

		for (int i = 0; i < toConvertSize; i++)
		{
			toReturn[i] = toConvert[i];
		}

		toReturn[toConvertSize] = '\0';

		return toReturn;
	}
}