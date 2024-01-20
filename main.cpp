#include <iostream>
#include <vector>
#include <algorithm>
#include <dirent.h>
#include <locale.h>

#include "trie/def.hpp"
#include "trie/exceptions.hpp"
#include "trie/extended_character_functions.hpp"
#include "trie/trie_node.hpp"
#include "trie/trie.hpp"

void pick_dictionary( std::string&);
bool parse_input( std::string&, std::string&, std::string&);

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "");

	std::string dictionary_name;
	triectionary::Trie* t;

	bool successful_setup = false;
	while (!successful_setup)
	{
		// read dictionary name from user
		pick_dictionary( dictionary_name );

		// set up the Trie
		try
		{
			printf("\nBringing data in memory...\n");
			t = new triectionary::Trie( dictionary_name );
			successful_setup = true;
			printf("Trie set up successfully :)\n\n");
		}
		catch (triectionary::ErrorOpeningDictionaryException eode)
		{
			successful_setup = false;
			std::cout << eode.info() << std::endl;
		}
		catch (triectionary::ErrorReadingDictionaryException erde)
		{
			successful_setup = false;
			std::cout << erde.info() << std::endl;
		}
	}

	// start reading input commands from the user
	std::string input, input1, input2;
	character_t *arg1, *arg2;
	bool correct_input;
	do
	{
		printf("Main menu:\n");
		printf("\\a w -> t   ||| add word w with translation t in the dictionary\n");
		printf("\\s w        ||| search for the translation of word w\n");
		printf("\\d w        ||| delete word w and its translation\n");
		printf("\\c          ||| get total number of saved translations in Trie\n");
		printf("\\i filename ||| import csv file of format (word,translation)\n");
		printf("\\z filename ||| delete csv file of format (word,translation) - translation is ignored, used mainly for debugging\n");
		printf("\\e 1        ||| exit and save changes\n");
		printf("\\e 0        ||| exit without saving changes\n");

		input.clear();
		input1.clear();
		input2.clear();
		arg1 = NULL;
		arg2 = NULL;
		std::getline(std::cin, input);

		correct_input = parse_input( input, input1, input2);

		if (correct_input)
		{
			if (!input.compare("\\a"))
			{
				printf("inserting: (word -> %s) and (translation -> %s)\n", input1.c_str(), input2.c_str() );

				arg1 = triectionary::str_to_c( input1 );
				arg2 = triectionary::str_to_c( input2 );

				if ( t->add_word( arg1, arg2 ) != NULL )
					printf("Added word %s with translation %s successfully in Trie\n\n", input1.c_str(), input2.c_str() );
				else
					printf("Translation already exists for this word\n\n");

				delete[] arg1;
				delete[] arg2;
			}
			else if (!input.compare("\\s"))
			{
				printf("Searching: (word -> %s)\n", input1.c_str());

				arg1 = triectionary::str_to_c( input1 );

				arg2 = t->search_word( arg1 );

				if ( arg2 != NULL )
				{
					printf("%s -> ", input1.c_str());
					for (uint32_t i = 0; arg2[i] != END_OF_STRING; i++)
						printf("%lc", arg2[i]);
					printf("\n\n");
				}
				else
					printf("%s doesn't exist in this dictionary\n\n", input1.c_str());

				delete[] arg1;
			}
			else if (!input.compare("\\d"))
			{
				printf("deleting: (word -> %s)\n", input1.c_str());

				arg1 = triectionary::str_to_c( input1 );

				arg2 = t->delete_word( arg1);

				if ( arg2 != NULL )
				{
					printf("Deleted word %s with translation ", input1.c_str());
					for (uint32_t i = 0; arg2[i] != END_OF_STRING; i++)
						printf("%lc", arg2[i]);
					printf(" successfully from Trie\n\n");
				}
				else
					printf("%s doesn't exist in this dictionary\n\n", input1.c_str());

				delete[] arg1;
				delete[] arg2;
			}
			else if (!input.compare("\\c"))
			{
				printf("Totally %d entries in Trie\n\n", t->get_entry_count());
			}
			else if (!input.compare("\\i"))
			{
				try
				{
					t->insert_from_csv(input1);
				}
				catch (triectionary::ErrorOpeningCsvException eoce)
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
				catch (triectionary::ErrorOpeningCsvException eoce)
				{
					std::cout << eoce.info() << std::endl;
				}
				printf("\n");
			}
			else if (!input.compare("\\e"))
			{
				if (!input1.compare("1"))
					t->set_saving_changes(true);
				break;
			}
		}
	}
	while(true);

	// destroy memory used for Trie
	delete t;

	return 0;
}

void pick_dictionary( std::string& dictionary_name )
{
	// show available dictionaries (only in the /dictionaries/ directory)
	printf("Welcome, these are the available dictionaries:\n\n");
	std::vector<std::string> dictionaries;
	DIR *d;
	struct dirent *dir;
	d = opendir("./dictionaries");
	if (!d)
	{
		printf("Error while reading dictionaries directory\n");
		exit(-1);
	}
	while ((dir = readdir(d)) != NULL)
	{
		if ( std::string(dir->d_name).compare(".") && std::string(dir->d_name).compare("..") )
		{
			std::cout << dir->d_name << std::endl;
			dictionaries.push_back(dir->d_name);
		}
	}
	closedir(d);

	// give dictionary name, create it if it doesn't exist
	printf("Please give a dictionary name to work with: ");
	std::string given_dictionary;
	std::getline(std::cin, given_dictionary);
	if(std::find(dictionaries.begin(), dictionaries.end(), given_dictionary) == dictionaries.end())
	{
		printf("\nDictionary given NOT found\n");
		printf("Creating new dictionary file...\n");

		FILE* file = fopen(("./dictionaries/" + given_dictionary).c_str(), "wb");
		if (file == NULL)
		{
			fprintf(stderr, "Error opening file %s\n", ("./dictionaries/" + given_dictionary).c_str());
			exit(-1);
		}

		uint32_t character_size = CHARACTER_BYTES;
		fwrite( &character_size, sizeof(uint32_t), 1, file);

		uint32_t entry_count = 0;
		fwrite( &entry_count, sizeof(uint32_t), 1, file);

		fclose(file);

		printf("Dictionary file created successfully\n");
	}

	dictionary_name = "./dictionaries/" + given_dictionary;
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
		if (!strings[0].compare("\\a"))
		{
			input = "\\a";
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
		}
		else if ( (!strings[0].compare("\\s")) || (!strings[0].compare("\\d")) || (!strings[0].compare("\\i")) || (!strings[0].compare("\\z"))  )
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
		else if (!strings[0].compare("\\e"))
		{
			if ((strings.size() == 2) && ((!strings[1].compare("1")) || !strings[1].compare("0")) )
			{
				input = "\\e";
				input1 = strings[1];
				input2 = "";

				if ( ! ((!input1.compare("0")) || (!input1.compare("1"))) )
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
			printf("Wrong input given\n\n");
			return false;
		}
	}
	else
	{
		if (!strings[0].compare("\\c"))
		{
			input = "\\c";
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
