#ifndef TRIECTIONARY_DEF_H_
#define TRIECTIONARY_DEF_H_

#include <stdint.h>
#include <string>

#define CHARACTER_BYTES 1

/* make sure that you only have character sizes of either 1, 2 or 4 bytes */
static_assert( (CHARACTER_BYTES == 1) || (CHARACTER_BYTES == 2) || (CHARACTER_BYTES == 4),
	"You can only have the following character byte sizes: 1, 2, 4\n");

/* choosing the correct alphabet size and character data type */
#if 	CHARACTER_BYTES == 1
#define TOTAL_ALPHABET_SIZE 128
typedef uint8_t character_t;

#elif 	CHARACTER_BYTES == 2
#define TOTAL_ALPHABET_SIZE 65536
typedef uint16_t character_t;

#elif	CHARACTER_BYTES == 4
#define TOTAL_ALPHABET_SIZE 4294967296
typedef uint32_t character_t;

#endif

#endif