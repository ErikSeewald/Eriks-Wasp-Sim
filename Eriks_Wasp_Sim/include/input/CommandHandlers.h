#include <string>

/**
* @namespace CommandHandlers
* @brief A namespace for all command handling methods.
*/
namespace CommandHandlers
{
	void commandHelp(const std::string&);

	//SYNTAX
	void commandSyntax(const std::string&);

	void commandPosition(const std::string&);

	void commandEntity(const std::string&);

	//WASP
	void commandWasp(const std::string&);

	void commandWaspHelp(const std::string&);

	//SPAWN
	void commandSpawn(const std::string&);
}