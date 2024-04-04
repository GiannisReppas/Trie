#include "trie/string.hpp"
#include "trie/trie.hpp"

#include <gtest/gtest.h>

#include <vector>

TEST(StringTests, Strlen)
{
	std::vector<uint8_t> v;

	// normal check
	v = {1,2,3,0};
	EXPECT_EQ( 3 , trie::strlen( v ) );

	// 0 check
	v = {0};
	EXPECT_EQ( 0 , trie::strlen( v ) );
}

TEST(StringTests, Strcmp)
{
	std::vector<uint8_t> v1;
	std::vector<uint8_t> v2;

	// check v1 < v2
	v1 = {1,2,3,0};
	v2 = {1,2,4,0};
	EXPECT_EQ( -1 , trie::strcmp( v1, v2 ) );

	// check v1 > v2
	v1 = {1,2,4,0};
	v2 = {1,2,3,0};
	EXPECT_EQ( 1 , trie::strcmp( v1, v2 ) );

	// check v1 = v2
	v1 = {1,2,3,0};
	v2 = {1,2,3,0};
	EXPECT_EQ( 0 , trie::strcmp( v1, v2 ) );
}