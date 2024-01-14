#include <climits>

/*------------------------------------------------------------------------------------------------*/

/* need to define the full alphabet size it like this, to perform compile-time check */
#define TOTAL_ALPHABET_SIZE 128

/* make sure at compilation that:
	the machine contains 8 bits for 1 byte
	that the total alphabet size (real alphabet size + padding) is a multile of 8 */
static_assert(CHAR_BIT == 8, "Number of bits in a byte must be 8");
static_assert(TOTAL_ALPHABET_SIZE%8 == 0, "Alphabet size must be multiple of 8");

/*------------------------------------------------------------------------------------------------*/

typedef class TrieNode
{
public:
	/* stable size of (TOTAL_ALPHABET_SIZE/CHAR_BIT)*sizeof(unsigned char) bytes
	 unsigned char always 1 byte,
	 compilation fails if CPU doesn't have 8 bits in a byte,
	 compilation fails if TOTAL_ALPHABET_SIZE isn't multiple of 8 */
	unsigned char active_letters[TOTAL_ALPHABET_SIZE/CHAR_BIT];

	/* variable size (0 to TOTAL_ALPHABET_SIZE*sizeof(pointer)) bytes
	 pointer usually 8 bytes */
	class TrieNode **children;

	/* variable size, (0 to translation_size*sizeof(char)+sizeof(char)) bytes
	 char always 1 byte */
	char *translation;

	TrieNode();
	~TrieNode();

	int get_actives_count();
	TrieNode* get_node_if_possible( char );
	TrieNode* insert_letter( char );
	void set_child_null( char );
}TrieNode;

/*------------------------------------------------------------------------------------------------*/