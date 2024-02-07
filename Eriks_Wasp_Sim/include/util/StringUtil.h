#pragma once

#include <string>

/**
* @namespace StringUtil
* @brief A namespace for basic string utility functions used in the Wasp Sim.
*/
namespace StringUtil
{
	/**
	* Returns the first word in the given string or an empty string if no word is found.
	* A word is defined as a string of characters separated by whitespace.
	*
	* @param str the string to get the first word from
	* @return The first word in the given string or an empty string
	*/
	std::string getFirstWord(const std::string& str);

	std::string trimLeadingWhitespace(const std::string& str);

	std::string cutFirstWord(const std::string& str);

	bool isBlank(const std::string& str);
}