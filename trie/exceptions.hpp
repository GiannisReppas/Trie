#ifndef TRIECTIONARY_EXCEPTIONS_H_
#define TRIECTIONARY_EXCEPTIONS_H_

#include <iostream>
#include <string>

#include "def.hpp"

namespace triectionary
{

class ErrorOpeningDictionaryException : std::exception
{
private:
	std::string dictionary_name;
public:
	ErrorOpeningDictionaryException(std::string);
	std::string info();
};

class ErrorReadingDictionaryException : std::exception
{
private:
	std::string dictionary_name;
	std::string cause;
public:
	ErrorReadingDictionaryException( std::string, std::string);
	std::string info();
};

class ErrorOpeningCsvException : std::exception
{
private:
	std::string csv_name;
public:
	ErrorOpeningCsvException( std::string);
	std::string info();
};

}

#endif