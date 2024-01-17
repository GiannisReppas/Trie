#ifndef TRIECTIONARY_FUNCTIONS_H_
#define TRIECTIONARY_FUNCTIONS_H_

#include "def.hpp"

namespace triectionary
{

uint32_t strlen( const character_t* );
character_t* strcpy( character_t*, const character_t*);
character_t* str_to_c( std::string );
std::wstring c_to_wstr( const character_t* );

}

#endif