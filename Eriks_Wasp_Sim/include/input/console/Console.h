#pragma once

#include <iostream>
#include "json.hpp"
using nlohmann::json;

using CommandHandler = std::function<void (const std::string&)>;
using CommandHandlerMap = std::unordered_map<std::string, CommandHandler>;

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
	* Tries to process the given subcommand by finding a fitting command handler in the CommandHandlerMap
	* of the subcommands parent command.
	* Prints an error message if no fitting command handler is found.
	*
	* @param subcommand the subcommand to process
	* @param commandHandlers the CommandHandlerMap for the parent command
	*/
	void processCommand(const std::string& subcommand, CommandHandlerMap& commandHandlers);

	const json& getCommands();

	void _init();
}