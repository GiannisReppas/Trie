#ifndef TRIE_EXCEPTIONS_H_
#define TRIE_EXCEPTIONS_H_

#include <string>

namespace trie
{

class ErrorCreatingTrieException : std::exception
{
public:
	std::string info()
	{
		return "The given byte size for characters is not supported";
	}
};

class ErrorOpeningDictionaryException : std::exception
{
private:
	std::string dictionary_name;

public:
	ErrorOpeningDictionaryException(std::string d_n) : dictionary_name(d_n) {}

	std::string info()
	{
		return "Error opening the dictionary file named " + this->dictionary_name;
	}
};


class ErrorReadingDictionaryException : std::exception
{
private:
	std::string dictionary_name;
	std::string cause;

public:
	ErrorReadingDictionaryException( std::string d_n, std::string c) : dictionary_name(d_n), cause(c) {}

	std::string info()
	{
		return "Error reading the dictionary file named " + this->dictionary_name + " - " + this->cause;
	}
};


class ErrorOpeningCsvException : std::exception
{
private:
	std::string csv_name;

public:
	ErrorOpeningCsvException( std::string c_n) : csv_name(c_n) {}

	std::string info()
	{
		return "Error opening the csv file named " + this->csv_name;
	}
};

}

#endif