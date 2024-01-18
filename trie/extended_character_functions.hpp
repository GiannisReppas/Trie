#ifndef TRIECTIONARY_FUNCTIONS_H_
#define TRIECTIONARY_FUNCTIONS_H_

#include "def.hpp"

namespace triectionary
{

/* reports number of elements in the array until the first END_OF_STRING */
uint32_t strlen( const character_t* );

/* copies elements from second to first array, which is preallocated, until the first END_OF_STRING */
character_t* strcpy( character_t*, const character_t*);

/* reads an std::string and writes its elements to a character_t*
	be carefull, END_OF_STRING is '\0' for std::string */
character_t* str_to_c( std::string );

}

#endif