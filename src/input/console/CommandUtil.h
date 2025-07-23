#pragma once

#include "Simulation.h"

#include "json.hpp"
using nlohmann::json;

#include <glm/glm.hpp>

using CommandHandler = std::function<void(const std::string&)>;
using CommandHandlerMap = std::unordered_map<std::string, CommandHandler>;

/**
* @namespace CommandUtil
* @brief A namespace for utility functions regarding console commands.
*/
namespace CommandUtil
{
	static const glm::vec3 infVec =
		glm::vec3(std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity());
	static const std::string noWaspSelectedPrint = "No wasp selected";

	enum CommandEntity
	{
		WASP, FOOD, INVALID
	};

	CommandEntity convertToEntity(const std::string& subcommand);

	Strategies::SpawnStrategy convertToSpawnStrategy(const std::string& subcommand);

	Strategies::KillStrategy convertToKillStrategy(const std::string& subcommand);

	glm::vec3 convertToPosition(const std::string& subcommand);

	int convertToInt(const std::string& subcommand);

	float convertToFloat(const std::string& subcommand);

	json getCommandJson(const std::string& subcommand);

	/**
	* Prints the name and description of the given command.
	* Leads to a runtime error if the given json is invalid.
	*
	* @param command the command to print
	*/
	void printCommandDescription(const json& command);

	/**
	* Prints the syntax of the given command.
	* Leads to a runtime error if the given json is invalid.
	*
	* @param command the command to print the syntax of
	*/
	void printCommandSyntax(const json& command);

	/**
	* Prints all subcommands of the parent command, provided it can be found.
	*
	* @param parentCommandName the 'name' value of the parent command
	*/
	void printSubCommands(const json& parentCommandName);

	void printInvalidSyntaxError();

	void printError(std::string message);
}