#pragma once

#include <string>

/**
* @namespace OtherCommandHandlers
* @brief A namespace for all command handling methods that do not have their own files.
*/
namespace OtherCommandHandlers
{
	//SPAWN
	void commandSpawn(const std::string& subcommand);

	//KILL
	void commandKill(const std::string& subcommand);
}