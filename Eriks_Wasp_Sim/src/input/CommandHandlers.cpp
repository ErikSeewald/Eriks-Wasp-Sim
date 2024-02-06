#include "CommandHandlers.h"
#include "Console.h"

#include <iostream>
#include "json.hpp"
using nlohmann::json;

void CommandHandlers::commandHelp()
{
    for (const json& commandJson : Console::getCommands())
    {
        // extract json values like this because '<<' puts quotation marks around normal json values
        std::string name = commandJson["name"].get<std::string>();
        std::string description = commandJson["description"].get<std::string>();

        std::cout << "\n " << name << "\t\t" << description << std::endl;
    }
}