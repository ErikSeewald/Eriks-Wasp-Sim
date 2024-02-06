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
}
