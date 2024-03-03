#include "CommandUtil.h"
#include "JsonHandler.h"
#include "SimVisualizer.h"
#include "UI.h"
#include "Console.h"
#include "StringUtil.h"
#include <iostream>
#include <sstream>

const std::string invalidSyntaxPrint = "Invalid syntax!";
const std::string syntaxNotFoundPrint = "No syntax for this command found";
const json invalidJson = json();

using CommandUtil::CommandEntity;
CommandEntity CommandUtil::convertToEntity(const std::string& subcommand)
{
    if (subcommand == "wasp")
    {
        return CommandEntity::WASP;
    }
    return CommandEntity::INVALID;
}

using Simulation::SpawnStrategy;
SpawnStrategy CommandUtil::convertToSpawnStrategy(const std::string& subcommand)
{
    if (subcommand == "point")
    {
        return SpawnStrategy::POINT;
    }

    else if (subcommand == "random")
    {
        return SpawnStrategy::RANDOM;
    }

    return SpawnStrategy::INVALID;
}

using Simulation::KillStrategy;
KillStrategy CommandUtil::convertToKillStrategy(const std::string& subcommand)
{
    if (subcommand == "random")
    {
        return KillStrategy::RANDOM;
    }

    return KillStrategy::INVALID;
}

glm::vec3 CommandUtil::convertToPosition(const std::string& subcommand)
{
    static const std::string hereString = "here";
    static const std::string selectedString = "selected";

    if (subcommand == hereString)
    {
        return SimVisualizer::getCamera().position;
    }

    if (subcommand == selectedString)
    {
        Wasp* selectedWasp = UI::getUIState()->selectedWasp;
        if (selectedWasp != NULL)
        {
            return selectedWasp->getPosition();
        }
        else
        {
            printError(noWaspSelectedPrint);
            return infVec;
        }
    }

    // OTHERWISE: x,y,z COORDINATES
    std::istringstream stream(subcommand);
    float x, y, z;
    char comma1, comma2;

    stream >> x >> comma1 >> y >> comma2 >> z;

    // Check if parsing succeeded and commas are in place
    if (!stream.fail() && comma1 == ',' && comma2 == ',' && stream.eof())
    {
        return glm::vec3(x, y, z);
    }
    else
    {
        return infVec;
    }
}

int CommandUtil::convertToAmount(const std::string& subcommand)
{
    try
    {
        return std::stoi(subcommand);
    }
    catch (std::invalid_argument& e)
    {
        return -1;
    }
    catch (std::out_of_range& e)
    {
        return -1;
    }
}

json CommandUtil::getCommandJson(const std::string& subcommand)
{
    // Find the json command object for the subcommand string by walking through the json hierarchy
    // one word of the string at a time.

    //FIND BASE COMMAND
    std::string curCommandString = subcommand;
    std::string firstWord = StringUtil::getFirstWord(curCommandString);
    json curCommand = JsonHandler::findByName(Console::getCommands(), firstWord);

    //GO THROUGH SUBCOMMANDS
    while (!curCommandString.empty())
    {
        // cut the last parent command out of the curCommandString
        curCommandString = StringUtil::cutFirstWord(curCommandString);

        firstWord = StringUtil::getFirstWord(curCommandString);
        if (firstWord.empty()) { break; }

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
            printInvalidSyntaxError();
            return invalidJson;
        }
    }

    return curCommand;
}

/**
* Prints the name and description of the given command.
* Leads to a runtime error if the given json is invalid.
*
* @param command the command to print
*/
void CommandUtil::printCommandDescription(const json& command)
{
    // extract json values like this because '<<' puts quotation marks around normal json values
    std::string name = command["name"].get<std::string>();
    std::string description = command["description"].get<std::string>();

    std::cout << "\n " << name;

    int tabCount = 4 - (name.length() / 3.2);
    for (int i = 0; i < tabCount; i++)
    {
        std::cout << "\t";
    }

    std::cout << description << std::endl;
}

/**
* Prints the syntax of the given command.
* Leads to a runtime error if the given json is invalid.
*
* @param command the command to print the syntax of
*/
void CommandUtil::printCommandSyntax(const json& command)
{
    static const std::string syntaxKey = "syntax";
    if (!JsonHandler::hasKey(command, syntaxKey))
    {
        std::cout << syntaxNotFoundPrint << std::endl;
        return;
    }

    std::string syntax = command[syntaxKey].get<std::string>();
    std::cout << syntax << std::endl;
}

/**
* Prints all subcommands of the parent command, provided it can be found.
*
* @param parentCommandName the 'name' value of the parent command
*/
void CommandUtil::printSubCommands(const json& parentCommandName)
{
    static const std::string subcommandsKey = "subcommands";

    const json& command = JsonHandler::findByName(Console::getCommands(), parentCommandName);
    if (!JsonHandler::hasKey(command, subcommandsKey))
    {
        return;
    }

    for (const json& subcommand : command[subcommandsKey])
    {
        printCommandDescription(subcommand);
    }
}

void CommandUtil::printInvalidSyntaxError()
{
    std::cout << invalidSyntaxPrint << std::endl;
}

void CommandUtil::printError(const std::string& message)
{
    std::cout << "ERROR: " << message << std::endl;
}