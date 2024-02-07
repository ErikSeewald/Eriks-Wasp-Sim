#include "CommandHandlers.h"
#include "Console.h"
#include "JsonHandler.h"

#include <iostream>
#include "json.hpp"
using nlohmann::json;

void CommandHandlers::commandHelp(const std::string& subcommand)
{   
    if (!Console::isBlank(subcommand))
    {
        Console::printInvalidSyntaxError();
        return;
    }

    for (const json& commandJson : Console::getCommands())
    {
        Console::printCommandDescription(commandJson);
    }
}

//SYNTAX
void CommandHandlers::commandSyntax(const std::string& subcommand)
{
    std::string curCommandString = subcommand;

    // TURN 'syntax' INTO 'syntax syntax" so that it prints it's own syntax
    static const std::string syntaxCommandName = " syntax";
    if (Console::isBlank(subcommand))
    {
        curCommandString = syntaxCommandName;
    }

    // now find the json command object for the subcommand string by walking through the json hierarchy
    // one word of the string at a time. Then simply print out it's 'syntax' value

    //FIND BASE COMMAND
    std::string firstWord = Console::getFirstWord(curCommandString);
    json curCommand = JsonHandler::findByName(Console::getCommands(), firstWord);

    //GO THROUGH SUBCOMMANDS
    while (!curCommandString.empty())
    {
        // cut the last parent command out of the curCommandString
        curCommandString = Console::trimLeadingWhitespace(curCommandString);
        curCommandString = curCommandString.substr(firstWord.length());

        firstWord = Console::getFirstWord(curCommandString);
        if (firstWord.empty()) {break;}
        
        // look for a subcommand with a name equal to firstWord
        bool subcommandFound = false;
        for (const json& subcommand : curCommand["subcommands"])
        {
            if (subcommand["name"] == firstWord)
            {
                curCommand = subcommand;
                subcommandFound = true;
                break;
            }
        }

        if (!subcommandFound) 
        {
            Console::printInvalidSyntaxError();
            return;
        }
    }

    Console::printCommandSyntax(curCommand);
}

void CommandHandlers::commandPosition(const std::string&)
{
    static const std::string positionSyntaxCommand = " position";
    commandSyntax(positionSyntaxCommand);
}

void CommandHandlers::commandEntity(const std::string&)
{
    static const std::string entitySyntaxCommand = " entity";
    commandSyntax(entitySyntaxCommand);
}

//WASP
static const std::string waspCommandName = "wasp";
CommandHandlerMap waspCommandHandlers =
{
        {"help", CommandHandlers::commandWaspHelp}
};

void CommandHandlers::commandWasp(const std::string& subcommand)
{
    Console::processCommand(subcommand, waspCommandHandlers);
}

void CommandHandlers::commandWaspHelp(const std::string& subcommand)
{
    if (!Console::isBlank(subcommand))
    {
        Console::printInvalidSyntaxError();
        return;
    }

    Console::printSubCommands(waspCommandName);
}

//SPAWN
void CommandHandlers::commandSpawn(const std::string& subcommand)
{

}