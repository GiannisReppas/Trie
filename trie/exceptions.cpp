#include <string>
#include <iostream>

#include "def.hpp"
#include "exceptions.hpp"

namespace triectionary
{

/* ErrorOpeningDictionaryException */

ErrorOpeningDictionaryException::ErrorOpeningDictionaryException(std::string dictionary_name)
{
	this->dictionary_name = dictionary_name;
}

std::string ErrorOpeningDictionaryException::info()
{
	return "Error opening the dictionary file named " + this->dictionary_name;
}


/* ErrorReadingDictionaryException */

ErrorReadingDictionaryException::ErrorReadingDictionaryException(std::string dictionary_name, std::string cause)
{
	this->dictionary_name = dictionary_name;
	this->cause = cause;
}

std::string ErrorReadingDictionaryException::info()
{
	return "Error reading the dictionary file named " + this->dictionary_name + " - " + this->cause;
}


/* ErrorOpeningCsvException */

ErrorOpeningCsvException::ErrorOpeningCsvException(std::string csv_name)
{
	this->csv_name = csv_name;
}

std::string ErrorOpeningCsvException::info()
{
	return "Error opening the csv file named " + this->csv_name;
}

}