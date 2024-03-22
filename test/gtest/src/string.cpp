#include "trie/string.hpp"
#include "trie/trie.hpp"

#include <gtest/gtest.h>

#include <vector>

TEST(TrieTests, TempTest1)
{
	std::vector<uint8_t> v = {1,2,3,0};
	EXPECT_EQ( 3 , trie::strlen( v ) );
}
