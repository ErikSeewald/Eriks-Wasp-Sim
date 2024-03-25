#pragma once

#include <string>

/**
* @namespace WaspCommandHandlers
* @brief A namespace for all command handling methods related to commands starting with 'wasp'.
*/
namespace WaspCommandHandlers
{
	void commandWasp(const std::string& subcommand);

	void commandWaspHelp(const std::string& subcommand);

	void commandWaspKill(const std::string& subcommand);

	void commandWaspSetPos(const std::string& subcommand);

	void commandWaspSetHp(const std::string& subcommand);

	void commandWaspSetHunger(const std::string& subcommand);
}