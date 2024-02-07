#include "CommandHandlers.h"
#include "Console.h"
#include "JsonHandler.h"
#include "UI.h"
#include "SimVisualizer.h"
#include "Simulation.h"
#include "StringUtil.h"
#include <sstream>
#include "json.hpp"
using nlohmann::json;

static const glm::vec3 infVec =
glm::vec3(std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity());

static const std::string noWaspSelectedPrint = "No wasp selected";

void CommandHandlers::commandHelp(const std::string& subcommand)
{   
    if (!StringUtil::isBlank(subcommand))
    {
        Console::printInvalidSyntaxError();
        return;
    }

    for (const json& commandJson : Console::getCommands())
    {
        Console::printCommandDescription(commandJson);
    }
}

glm::vec3 CommandHandlers::_convertToPosition(const std::string& subcommand)
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
            std::cout << noWaspSelectedPrint << std::endl;
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

int CommandHandlers::_convertToAmount(const std::string& subcommand)
{
    try
    {
        return std::stoi(subcommand);
    }
    catch (std::invalid_argument& e)
    {
        return -1;
    }
}

//SYNTAX
void CommandHandlers::commandSyntax(const std::string& subcommand)
{
    std::string curCommandString = subcommand;

    // TURN 'syntax' INTO 'syntax syntax" so that it prints it's own syntax
    static const std::string syntaxCommandName = " syntax";
    if (StringUtil::isBlank(subcommand))
    {
        curCommandString = syntaxCommandName;
    }

    // now find the json command object for the subcommand string by walking through the json hierarchy
    // one word of the string at a time. Then simply print out it's 'syntax' value

    //FIND BASE COMMAND
    std::string firstWord = StringUtil::getFirstWord(curCommandString);
    json curCommand = JsonHandler::findByName(Console::getCommands(), firstWord);

    //GO THROUGH SUBCOMMANDS
    while (!curCommandString.empty())
    {
        // cut the last parent command out of the curCommandString
        curCommandString = StringUtil::cutFirstWord(curCommandString);

        firstWord = StringUtil::getFirstWord(curCommandString);
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
        {"help", CommandHandlers::commandWaspHelp},
        {"kill", CommandHandlers::commandWaspKill},
        {"setpos", CommandHandlers::commandWaspSetPos}
};

void CommandHandlers::commandWasp(const std::string& subcommand)
{
    Console::processCommand(subcommand, waspCommandHandlers);
}

void CommandHandlers::commandWaspHelp(const std::string& subcommand)
{
    if (!StringUtil::isBlank(subcommand))
    {
        Console::printInvalidSyntaxError();
        return;
    }

    Console::printSubCommands(waspCommandName);
}

void CommandHandlers::commandWaspKill(const std::string& subcommand)
{
    static const std::string successfulKillPrint = "Wasp killed successfully!";

    if (!StringUtil::isBlank(subcommand))
    {
        Console::printInvalidSyntaxError();
        return;
    }

    Wasp* selectedWasp = UI::getUIState()->selectedWasp;
    if (selectedWasp != NULL)
    {
        selectedWasp->kill();
        std::cout << successfulKillPrint;
    }
    else
    {
        std::cout << noWaspSelectedPrint;
    }
}

void CommandHandlers::commandWaspSetPos(const std::string& subcommand)
{
    std::string posString = StringUtil::getFirstWord(subcommand);

    if (posString.empty() || !StringUtil::isBlank(StringUtil::cutFirstWord(subcommand)))
    {
        Console::printInvalidSyntaxError();
        return;
    }

    glm::vec3 newPos = _convertToPosition(posString);
    if (newPos == infVec)
    {
        Console::printInvalidSyntaxError();
        return;
    }

    Wasp* selectedWasp = UI::getUIState()->selectedWasp;
    if (selectedWasp != NULL)
    {
        selectedWasp->setPosition(newPos);
        std::cout << "Wasp's position set to " << newPos.x << "," << newPos.y << "," << newPos.z;
    }
    else
    {
        std::cout << noWaspSelectedPrint;
    }
}

//SPAWN
using CommandHandlers::SpawnEntity;
SpawnEntity CommandHandlers::_convertToSpawnEntity(const std::string& subcommand)
{
    if (subcommand == "wasp")
    {
        return SpawnEntity::WASP;
    }
    return SpawnEntity::INVALID;
}

void CommandHandlers::commandSpawn(const std::string& subcommand)
{
    std::string firstWord = StringUtil::getFirstWord(subcommand);

    //ENTITY
    SpawnEntity entity = _convertToSpawnEntity(firstWord);
    if (entity == SpawnEntity::INVALID)
    {
        Console::printInvalidSyntaxError();
        return;
    }

    //POSITION
    std::string curCommandString = StringUtil::cutFirstWord(subcommand);
    firstWord = StringUtil::getFirstWord(curCommandString);
    glm::vec3 spawnPos = _convertToPosition(firstWord);
    if (spawnPos == infVec)
    {
        Console::printInvalidSyntaxError();
        return;
    }

    //AMOUNT
    curCommandString = StringUtil::cutFirstWord(curCommandString);
    firstWord = StringUtil::getFirstWord(curCommandString);
    int amount = _convertToAmount(firstWord); 
    if (amount < 1 || !StringUtil::isBlank(StringUtil::cutFirstWord(curCommandString)))
    {
        Console::printInvalidSyntaxError();
        return;
    }

    //EXECUTE
    switch (entity)
    {
        case WASP: 
            bool success = Simulation::spawnWasps(spawnPos, amount);
            if (success)
            {
                std::cout << "Wasp: Spawned " << amount << " at " << spawnPos.x << "," << spawnPos.y << "," << spawnPos.z << ",";
            }
            else
            {
                Console::printInvalidSyntaxError();
            }
    }
}