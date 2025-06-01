#include "StringUtil.h"
#include <sstream>

/**
* Returns the first word in the given string or an empty string if no word is found.
* A word is defined as a string of characters separated by whitespace.
*
* @param str the string to get the first word from
* @return The first word in the given string or an empty string
*/
std::string StringUtil::getFirstWord(const std::string& str)
{
    std::istringstream iss(str + " "); //add " ". Otherwise it wont recognize strings that are made up of one word
    std::string word;
    iss >> word; //extracts chars from iss and stores them in word until a whitespace char is encountered
    return word;
}

std::string StringUtil::trimLeadingWhitespace(const std::string& str)
{
    for (int i = 0; i < str.length(); i++)
    {
        if (!std::isspace(str[i]))
        {
            return str.substr(i);
        }
    }
    return std::string();
}

std::string StringUtil::cutFirstWord(const std::string& str)
{
    //MOVE TO FIRST WORD
    std::string cutString = str;
    bool wordFound = false;
    for (int i = 0; i < str.length(); i++)
    {
        if (!std::isspace(str[i]))
        {
            cutString = str.substr(i, str.length());
            wordFound = true;
            break;
        }
    }

    if (!wordFound)
    {
        return std::string();
    }

    //CUT OUT FIRST WORD
    for (int i = 0; i < cutString.length(); i++)
    {
        if (std::isspace(cutString[i]))
        {
            return cutString.substr(i, cutString.length());
        }
    }
    return std::string();
}

bool StringUtil::isBlank(const std::string& str)
{
    return trimLeadingWhitespace(str).empty();
}