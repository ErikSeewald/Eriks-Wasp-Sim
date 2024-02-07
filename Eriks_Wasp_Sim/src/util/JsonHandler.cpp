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

/**
* Returns wether the given json contains a given key.
*
* @param findIn the json to find the key in
* @param key the key to find
* @return wether the given json contains the key
*/
bool JsonHandler::hasKey(const json& findIn, const std::string& key)
{
    try
    {
        findIn[key];
    }
    catch (const json::type_error& e)
    {
        return false;
    }
    return true;
}

/**
* Searches for a value by name in the given json. Returns a default json object if
* no value was found.
* 
* @param findIn the json to find the value in
* @param name the name of the value to find
* @return the value or a default json object if no value was found
*/
const json& JsonHandler::findByName(const json& findIn, const json& name)
{
    try
    {
        for (const json& j : findIn)
        {
            if (j["name"] == name)
            {
                return j;
            }
        }
    }
    catch (const json::type_error& e) {}

    return json();
}