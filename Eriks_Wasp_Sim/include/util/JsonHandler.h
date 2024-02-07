#pragma once

#include "json.hpp"
using nlohmann::json;

/**
* @namespace JsonHandler
* @brief A namespace for all methods handling json files.
*/
namespace JsonHandler
{
	/**
	* Loads and returns the json found at the given file location
	* 
	* @param filename the name of the file
	* @return the parsed json
	* @throws std::runtime_error if the file can not be found or parsed
	*/
	json loadJson(const std::string& filename);

	/**
	* Returns wether the given json contains a given key.
	* 
	* @param findIn the json to find the key in
	* @param key the key to find
	* @return wether the given json contains the key
	*/
	bool hasKey(const json& findIn, const std::string& key);

	/**
	* Searches for a value by name in the given json. Returns a default json object if
	* no value was found.
	*
	* @param findIn the json to find the value in
	* @param name the name of the value to find
	* @return the value or a default json object if no value was found
	*/
	const json& findByName(const json& findIn, const json& name);
}
