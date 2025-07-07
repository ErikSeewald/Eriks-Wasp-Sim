#pragma once

#include <string>

/**
* @namespace OtherCommandHandlers
* @brief A namespace for all command handling methods that do not have their own files.
*/
namespace OtherCommandHandlers
{
	void commandSpawn(const std::string& subcommand);

	void commandKill(const std::string& subcommand);

	void commandResourceSettings(const std::string& subcommand);
}