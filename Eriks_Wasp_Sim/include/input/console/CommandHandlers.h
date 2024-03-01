#pragma once

#include <string>

/**
* @namespace CommandHandlers
* @brief A namespace for all command handling methods.
*/
namespace CommandHandlers
{
	void commandHelp(const std::string& subcommand);

	//SYNTAX
	void commandSyntax(const std::string& subcommand);

	void commandElement(const std::string& subcommand);

	//WASP
	void commandWasp(const std::string& subcommand);

	void commandWaspHelp(const std::string& subcommand);

	void commandWaspKill(const std::string& subcommand);

	void commandWaspSetPos(const std::string& subcommand);

	//SPAWN
	void commandSpawn(const std::string& subcommand);
}