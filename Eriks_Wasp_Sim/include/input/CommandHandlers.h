#pragma once

#include <string>
#include <glm/glm.hpp>

/**
* @namespace CommandHandlers
* @brief A namespace for all command handling methods.
*/
namespace CommandHandlers
{
	void commandHelp(const std::string& subcommand);

	enum SpawnEntity
	{
		WASP, INVALID
	};

	SpawnEntity _convertToSpawnEntity(const std::string& subcommand);

	glm::vec3 _convertToPosition(const std::string& subcommand);

	int _convertToAmount(const std::string& subcommand);

	//SYNTAX
	void commandSyntax(const std::string& subcommand);

	void commandPosition(const std::string& subcommand);

	void commandEntity(const std::string& subcommand);

	//WASP
	void commandWasp(const std::string& subcommand);

	void commandWaspHelp(const std::string& subcommand);

	void commandWaspKill(const std::string& subcommand);

	void commandWaspSetPos(const std::string& subcommand);

	//SPAWN
	void commandSpawn(const std::string& subcommand);
}