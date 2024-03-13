#include <iostream>
#include <vector>
#include <fstream>
#include <string>

#include "trie/trie.hpp"

bool parse_input( std::string&, std::string&, std::string&);

int main(void)
{
	// load file in memory
	trie::Trie<uint8_t>* t;
	t = new trie::Trie<uint8_t>("trie_ascii");

	// read input from command line
	std::string input, input1, input2;
	bool correct_input;
	std::vector<uint8_t> arg1,arg2;
	std::vector< std::vector<uint8_t> > prefix_answers;
	do
	{
		printf("Main menu:\n");
		printf("\\a w -> t     ||| add word w with translation t in the dictionary\n");
		printf("\\s w          ||| search for the translation of word w\n");
		printf("\\d w          ||| delete word w and its translation\n");
		printf("\\p w -> count ||| get count words that begin with word w\n");
		printf("\\c            ||| get total number of saved translations in Trie\n");
		printf("\\i filename   ||| import csv file of format (word,translation)\n");
		printf("\\z filename   ||| delete csv file of format (word,translation) - translation is ignored, used mainly for debugging\n");
		printf("\\w            ||| write current trie information to the dictionary file\n");
		printf("\\e            ||| exit\n");

		// prepare for next command
		input.clear();
		input1.clear();
		input2.clear();
		arg1.clear();
		arg2.clear();
		prefix_answers.clear();

		// read next command
		std::getline(std::cin, input);

		// split input into basic parts
		correct_input = parse_input( input, input1, input2);

		// check if command given exists
		if (correct_input)
		{
			if (!input.compare("\\a"))
			{
				printf("inserting: (word -> %s) and (translation -> %s)\n", input1.c_str(), input2.c_str() );

				arg1.insert( arg1.end(), input1.begin(), input1.begin() + (input1.size() + 1) );
				arg2.insert( arg2.end(), input2.begin(), input2.begin() + (input2.size() + 1) );

				if ( t->add_word( arg1, arg2 ) )
					printf("Added word %s with translation %s successfully in Trie\n\n", input1.c_str(), input2.c_str() );
				else
					printf("Translation already exists for this word\n\n");
			}
			else if (!input.compare("\\s"))
			{
				printf("Searching: (word -> %s)\n", input1.c_str());

				arg1.insert( arg1.end(), input1.begin(), input1.begin() + (input1.size() + 1) );

				arg1 = t->search_word( arg1 );

				if ( arg1.size() != 0 )
				{
					printf("%s -> ", input1.c_str());
					for (uint32_t i=0; i < arg1.size(); i++ )
						printf("%c", arg1[i]);
					printf("\n\n");
				}
				else
					printf("%s doesn't exist in this dictionary\n\n", input1.c_str());
			}
			else if (!input.compare("\\d"))
			{
				printf("deleting: (word -> %s)\n", input1.c_str());

				arg1.insert( arg1.end(), input1.begin(), input1.begin() + (input1.size() + 1) );

				if ( t->delete_word( arg1 ) )
					printf("Deleted word %s successfully from Trie\n\n", input1.c_str());
				else
					printf("%s doesn't exist in this dictionary\n\n", input1.c_str());
			}
			else if (!input.compare("\\p"))
			{
				printf("Searching prefixes for: (word -> %s)\n", input1.c_str());

				arg1.insert( arg1.end(), input1.begin(), input1.begin() + (input1.size() + 1) );

				prefix_answers = t->get_prefix_words( arg1, stoi(input2) );

				printf("prefixes: ");
				for (uint8_t i = 0; i < prefix_answers.size(); i++)
				{
						for (uint8_t j = 0; prefix_answers[i][j] != '\0'; j++)
							printf("%lc", prefix_answers[i][j]);
						if (i != (prefix_answers.size()-1))
							printf(", ");
				}
				printf("\n\n");
			}
			else if (!input.compare("\\c"))
			{
				printf("Totally %ld entries in Trie\n\n", t->get_entry_count());
			}
			else if (!input.compare("\\i"))
			{
				try
				{
					t->insert_from_csv(input1);
				}
				catch (trie::ErrorOpeningCsvException eoce)
				{
					std::cout << eoce.info() << std::endl;
				}
				printf("\n");
			}
			else if (!input.compare("\\z"))
			{
				try
				{
					t->delete_from_csv(input1);
				}
				catch (trie::ErrorOpeningCsvException eoce)
				{
					std::cout << eoce.info() << std::endl;
				}
				printf("\n");
			}
			else if (!input.compare("\\w"))
			{
				try
				{
					t->save_changes();
				}
				catch (trie::ErrorOpeningDictionaryException eode)
				{
					std::cout << eode.info() << std::endl;
				}
				printf("\n");
			}
			else if (!input.compare("\\e"))
			{
				break;
			}
		}
	}
	while(true);

	// destroy memory used for Trie
	delete t;

	return 0;
}

bool parse_input( std::string& input, std::string& input1, std::string& input2)
{
	// split input string
	std::vector<std::string> strings;
	uint32_t startIndex = 0, endIndex = 0;
	for (uint32_t i = 0; i <= input.size(); i++)
	{
		if (input[i] == ' ' || i == input.size())
		{
			endIndex = i;
			std::string temp;
			temp.append( input, startIndex, endIndex - startIndex);
			strings.push_back(temp);
			startIndex = endIndex + 1;
		}
	}

	// check if input given is correct
	if (strings.size() > 1)
	{
		if ( (!strings[0].compare("\\a")) || (!strings[0].compare("\\p")) )
		{
			input = strings[0];
			input1 = "";
			uint32_t i;
			bool arrow_flag = false;
			for (i=1; i < strings.size(); i++)
			{
				if (!strings[i].compare("->"))
				{
					arrow_flag = true;
					break;
				}
				input1 += strings[i] + " ";
			}
			if (!arrow_flag)
			{
				printf("Wrong input given\n\n");
				return false;
			}
			if (input1.compare(""))
				input1.pop_back();
			for (i=i+1; i < strings.size(); i++)
			{
				if (!strings[i].compare("->"))
					break;
				input2 += strings[i] + " ";
			}
			if (input2.compare(""))
				input2.pop_back();

			if ( (!input1.compare("")) || (!input2.compare("")) )
			{
				printf("Wrong input given\n\n");
				return false;
			}

			if ( (!strings[0].compare("\\p")) )
			{
				try
				{
					stoi(input2);
				}
				catch (std::invalid_argument const& ex)
				{
					printf("Wrong input given\n\n");
					return false;
				}
			}
		}
		else if ( (!strings[0].compare("\\s")) || (!strings[0].compare("\\d")) || (!strings[0].compare("\\i")) || (!strings[0].compare("\\z")) )
		{
			input = strings[0];
			input1 = "";
			for (uint32_t i=1; i < strings.size(); i++)
				input1 += strings[i] + " ";
			if (input1.compare(""))
				input1.pop_back();
			input2 = "";

			if (!input1.compare(""))
			{
				printf("Wrong input given\n\n");
				return false;
			}
		}
		else
		{
			printf("Wrong input given\n\n");
			return false;
		}
	}
	else
	{
		if ((!strings[0].compare("\\c")) || (!strings[0].compare("\\e")) || (!strings[0].compare("\\w")))
		{
			input = strings[0];
			input1 = "";
			input2 = "";
		}
		else
		{
			printf("Wrong input given\n\n");
			return false;
		}
	}

	return true;
}
