#include "JsonHandler.h"
#include <fstream>

/**
* Loads and returns the json found at the given file location
*
* @param filename the name of the file
* @return the parsed json
* @throws std::runtime_error if the file can not be found or parsed
*/
json JsonHandler::loadJson(const std::string& filename)
{
    std::ifstream file(filename);
    json j;

    if (!file.is_open())
    {
        throw std::runtime_error("Unable to load file: " + filename);
    }

    try
    {
        file >> j;
    }
    catch (const json::parse_error& e)
    {
        throw std::runtime_error("JSON parsing error: " + std::string(e.what()));
    }

    return j;
}