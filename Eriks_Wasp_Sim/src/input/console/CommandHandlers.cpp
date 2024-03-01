#include "CommandHandlers.h"
#include "Console.h"
#include "JsonHandler.h"
#include "StringUtil.h"
#include "json.hpp"
#include "CommandUtil.h"
#include "UI.h"
#include "WaspSlots.h"

using nlohmann::json;
using CommandUtil::CommandEntity;
using Simulation::KillStrategy;
using WaspSlots::WaspSlot;

void CommandHandlers::commandHelp(const std::string& subcommand)
{   
    if (!StringUtil::isBlank(subcommand))
    {
        CommandUtil::printInvalidSyntaxError();
        return;
    }

    for (const json& commandJson : Console::getCommands())
    {
        CommandUtil::printCommandDescription(commandJson);
    }
}


//-------------------------------------
//----------------SYNTAX----------------
//-------------------------------------
void CommandHandlers::commandSyntax(const std::string& subcommand)
{
    std::string commandString = subcommand;

    if (StringUtil::isBlank(subcommand))
    {
        // TURN 'syntax' INTO 'syntax syntax" so that it prints it's own syntax
        static const std::string syntaxCommandName = " syntax";
        commandString = syntaxCommandName;
    }
  
    const json& command = CommandUtil::getCommandJson(commandString);
    CommandUtil::printCommandSyntax(command);
}

void CommandHandlers::commandElement(const std::string& subcommand)
{
    std::string element = StringUtil::getFirstWord(subcommand);

    if (element.empty() || !StringUtil::isBlank(subcommand.substr(element.size() + 1)))
    {
        CommandUtil::printInvalidSyntaxError();
        return;
    }

    json elementsCommand = JsonHandler::findByName(Console::getCommands(), "element");
    json specificElement = JsonHandler::findByName(elementsCommand["elements"], element);

    if (specificElement.empty())
    {
        CommandUtil::printInvalidSyntaxError();
        return;
    }

    CommandUtil::printCommandDescription(specificElement);
}


//-------------------------------------
//----------------WASP----------------
//-------------------------------------
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
        CommandUtil::printInvalidSyntaxError();
        return;
    }

    CommandUtil::printSubCommands(waspCommandName);
}

void CommandHandlers::commandWaspKill(const std::string& subcommand)
{
    static const std::string successfulKillPrint = "Wasp killed successfully!";

    if (!StringUtil::isBlank(subcommand))
    {
        CommandUtil::printInvalidSyntaxError();
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
        std::cout << CommandUtil::noWaspSelectedPrint;
    }
}

void CommandHandlers::commandWaspSetPos(const std::string& subcommand)
{
    //POSITION STRING
    std::string posString = StringUtil::getFirstWord(subcommand);
    if (posString.empty() || !StringUtil::isBlank(StringUtil::cutFirstWord(subcommand)))
    {
        CommandUtil::printInvalidSyntaxError();
        return;
    }

    //GET POSITION
    glm::vec3 newPos = CommandUtil::convertToPosition(posString);
    if (newPos == CommandUtil::infVec)
    {
        CommandUtil::printInvalidSyntaxError();
        return;
    }

    //SET POSITION
    Wasp* selectedWasp = UI::getUIState()->selectedWasp;
    if (selectedWasp != NULL)
    {
        selectedWasp->setPosition(newPos);
        std::cout << "Wasp's position set to " << newPos.x << "," << newPos.y << "," << newPos.z;
    }
    else
    {
        std::cout << CommandUtil::noWaspSelectedPrint;
    }
}


//-------------------------------------
//----------------SPAWN----------------
//-------------------------------------
void CommandHandlers::commandSpawn(const std::string& subcommand)
{
    std::string firstWord = StringUtil::getFirstWord(subcommand);

    //ENTITY
    CommandEntity entity = CommandUtil::convertToEntity(firstWord);
    if (entity == CommandEntity::INVALID)
    {
        CommandUtil::printInvalidSyntaxError();
        return;
    }

    //POSITION
    std::string curCommandString = StringUtil::cutFirstWord(subcommand);
    firstWord = StringUtil::getFirstWord(curCommandString);
    glm::vec3 spawnPos = CommandUtil::convertToPosition(firstWord);
    if (spawnPos == CommandUtil::infVec)
    {
        CommandUtil::printInvalidSyntaxError();
        return;
    }

    //AMOUNT
    curCommandString = StringUtil::cutFirstWord(curCommandString);
    firstWord = StringUtil::getFirstWord(curCommandString);
    int amount = CommandUtil::convertToAmount(firstWord); 
    if (amount < 1 || !StringUtil::isBlank(StringUtil::cutFirstWord(curCommandString)))
    {
        CommandUtil::printInvalidSyntaxError();
        return;
    }

    //EXECUTE
    switch (entity)
    {
        case CommandEntity::WASP: 
            bool success = Simulation::spawnWasps(spawnPos, amount);
            if (success)
            {
                std::cout << "Entity wasp: Spawned " << amount << " at " << spawnPos.x << "," << spawnPos.y << "," << spawnPos.z << ",";
            }
            else
            {
                CommandUtil::printInvalidSyntaxError();
            }
            break;
    }
}

//-------------------------------------
//----------------KILL----------------
//-------------------------------------
void CommandHandlers::commandKill(const std::string& subcommand)
{
    KillStrategy strategy{};
    std::string firstWord = StringUtil::getFirstWord(subcommand);

    //ENTITY
    CommandEntity entity = CommandUtil::convertToEntity(firstWord);
    if (entity == CommandEntity::INVALID)
    {
        CommandUtil::printInvalidSyntaxError();
        return;
    }

    //AMOUNT
    std::string curCommandString = StringUtil::cutFirstWord(subcommand);
    firstWord = StringUtil::getFirstWord(curCommandString);

    int amount = 0;
    if (firstWord == "all")
    {
        strategy = KillStrategy::ALL;
    }

    else if (firstWord == "selected")
    {
        commandWaspKill("");
        return;
    }

    else
    {
        amount = CommandUtil::convertToAmount(firstWord);
        if (amount < 1)
        {
            CommandUtil::printInvalidSyntaxError();
            return;
        }

        //KILL STRATEGY
        curCommandString = StringUtil::cutFirstWord(curCommandString);
        firstWord = StringUtil::getFirstWord(curCommandString);

        if (firstWord.empty())
        {
            strategy = KillStrategy::RANDOM;
        }

        else
        {
            strategy = CommandUtil::convertToKillStrategy(firstWord);

            if (strategy == KillStrategy::INVALID || !StringUtil::isBlank(StringUtil::cutFirstWord(curCommandString)))
            {
                CommandUtil::printInvalidSyntaxError();
                return;
            }
        }
    }

    //EXECUTE
    switch (entity)
    {
        case CommandEntity::WASP:
            int killedAmount = Simulation::killWasps(amount, strategy);
            std::cout << "Entity wasp: Killed " << killedAmount;
            break;
    }
}