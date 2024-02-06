#include <iostream>
#include <nlohmann/json.hpp>
using nlohmann::json;

/**
* @namespace Console
* @brief A namespace for all methods dealing with console input
*/
namespace Console  
{
	/**
	* Initializes the console and starts the loop needed to get continuous input from the user.
	*/
	void startLoop();

	/**
	* Tries to process the given command by finding a fitting command handler.
	* Prints an error message if no fitting command handler is found.
	*
	* @param command the command to process
	*/
	void processCommand(const std::string& command);

	const json& getCommands();

	void _init();

	/**
	* Returns the first word in the given string or an empty string if no word is found.
	* A word is defined as a string of characters separated by whitespace.
	*
	* @param str the string to get the first word from
	* @return The first word in the given string or an empty string
	*/
	std::string _getFirstWord(const std::string& str);
}