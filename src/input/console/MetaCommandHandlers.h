#pragma once

#include <string>

/**
* @namespace MetaCommandHandlers
* @brief A namespace for all command handling methods that relate to meta information about commands.
*/
namespace MetaCommandHandlers
{
	void commandHelp(const std::string& subcommand);

	void commandSyntax(const std::string& subcommand);

	void commandElement(const std::string& subcommand);
}