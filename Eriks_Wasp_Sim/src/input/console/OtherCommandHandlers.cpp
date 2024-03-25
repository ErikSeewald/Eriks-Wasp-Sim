#include "OtherCommandHandlers.h"
#include "WaspCommandHandlers.h"
#include "Console.h"
#include "StringUtil.h"
#include "CommandUtil.h"
#include "WaspSlots.h"
#include "MemoryManager.h"

using CommandUtil::CommandEntity;
using Simulation::KillStrategy;
using Simulation::SpawnStrategy;
using WaspSlots::WaspSlot;

//-------------------------------------
//----------------SPAWN----------------
//-------------------------------------
void OtherCommandHandlers::commandSpawn(const std::string& subcommand)
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
    int amount = CommandUtil::convertToNumber(firstWord); 
    if (amount < 1)
    {
        CommandUtil::printInvalidSyntaxError();
        return;
    }

    //STRATEGY
    SpawnStrategy strategy{};
    float spawnRadius = 0;
    curCommandString = StringUtil::cutFirstWord(curCommandString);
    firstWord = StringUtil::getFirstWord(curCommandString);

    if (firstWord.empty())
    {
        strategy = SpawnStrategy::POINT;
    }

    else
    {
        strategy = CommandUtil::convertToSpawnStrategy(firstWord);

        if (strategy == SpawnStrategy::RANDOM)
        {
            curCommandString = StringUtil::cutFirstWord(curCommandString);
            firstWord = StringUtil::getFirstWord(curCommandString);
            spawnRadius = CommandUtil::convertToNumber(firstWord);

            if (spawnRadius <= 0)
            {
                CommandUtil::printInvalidSyntaxError();
                return;
            }
        }

        else if (strategy == SpawnStrategy::INVALID || !StringUtil::isBlank(StringUtil::cutFirstWord(curCommandString)))
        {
            CommandUtil::printInvalidSyntaxError();
            return;
        }
    }

    //EXECUTE
    switch (entity)
    {
        case CommandEntity::WASP:
            bool success = WaspSlots::spawnWasps(spawnPos, amount, strategy, spawnRadius);
            if (success)
            {
                std::cout << "Entity wasp: Spawned " << amount << " at " << spawnPos.x << "," << spawnPos.y << "," << spawnPos.z << ",";
            }
            else
            {
                if (!WaspSlots::spaceAvailable(amount))
                {
                    CommandUtil::printError("Cannot surprass the maximum amount of wasps");
                    return;
                }
                CommandUtil::printInvalidSyntaxError();
            }
            break;
    }
}

//-------------------------------------
//----------------KILL----------------
//-------------------------------------
void OtherCommandHandlers::commandKill(const std::string& subcommand)
{
    // The kill command schedules a new cleanup. If the user was allowed to continue spamming kill constantly
    // it would cause the cleanup to never occur. Avoid that.
    if (MemoryManager::isCleanupScheduled())
    {
        CommandUtil::printError("Cannot invoke 'kill' while memory is still being cleaned. \nTry again in a few seconds.");
        return;
    }

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
        WaspCommandHandlers::commandWaspKill("");
        return;
    }

    else
    {
        amount = CommandUtil::convertToNumber(firstWord);
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
            int killedAmount = WaspSlots::killWasps(amount, strategy);
            std::cout << "Entity wasp: Killed " << killedAmount;
            break;
    }
}