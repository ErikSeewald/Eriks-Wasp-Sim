#include "OtherCommandHandlers.h"
#include "WaspCommandHandlers.h"
#include "Console.h"
#include "StringUtil.h"
#include "CommandUtil.h"
#include "WaspSlots.h"
#include "MemoryManager.h"
#include "Food.h"
#include "ResourceSpawner.h"
#include <set>

using CommandUtil::CommandEntity;
using Strategies::KillStrategy;
using Strategies::SpawnStrategy;
using ResourceSpawner::ResourceSettings;

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
    int amount = CommandUtil::convertToInt(firstWord); 
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
            spawnRadius = CommandUtil::convertToInt(firstWord);

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
    bool success;
    switch (entity)
    {
        case CommandEntity::WASP:
            success = WaspSlots::spawnWasps(spawnPos, amount, strategy, spawnRadius);
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

        case CommandEntity::FOOD:
            success = Food::spawnFood(spawnPos, amount, strategy, spawnRadius);
            if (success)
            {
                std::cout << "Entity food: Spawned " << amount << " at " << spawnPos.x << "," << spawnPos.y << "," << spawnPos.z << ",";
            }
            else
            {
                if (!Food::spaceAvailable(amount))
                {
                    CommandUtil::printError("Cannot surprass the maximum amount of food");
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
        amount = CommandUtil::convertToInt(firstWord);
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
    int killedAmount;
    switch (entity)
    {
        case CommandEntity::WASP:
            killedAmount = WaspSlots::killWasps(amount, strategy);
            std::cout << "Entity wasp: Killed " << killedAmount;
            break;

        case CommandEntity::FOOD:
            killedAmount = Food::killFood(amount, strategy);
            std::cout << "Entity food: Killed " << killedAmount;
            break;
    }
}

//-------------------------------------
//----------RESOURCE SETTINGS----------
//-------------------------------------
static const std::set<std::string> floatValueSettings = {"foodSpawnedPerSecond", "foodSpawnRadius"};

void OtherCommandHandlers::commandResourceSettings(const std::string& subcommand)
{
    //SETTING TYPE
    std::string type = StringUtil::getFirstWord(subcommand);

    //SETTING VALUE
    std::string curCommandString = StringUtil::cutFirstWord(subcommand);
    std::string value = StringUtil::getFirstWord(curCommandString);

    if (!StringUtil::isBlank(StringUtil::cutFirstWord(curCommandString)))
    {  
        CommandUtil::printInvalidSyntaxError();
        return;
    }

    //EXECUTE
    ResourceSettings* settings = ResourceSpawner::getResourceSettings();

    // Positive float vaue
    if (floatValueSettings.find(type) != floatValueSettings.end())
    {
        float f_value = CommandUtil::convertToFloat(value);

        if (f_value < 0)
        {
            CommandUtil::printError("Setting value cannot be < 0");
            return;
        }

        if (type == "foodSpawnedPerSecond")
        {
            settings->foodSpawnedPerSecond = f_value;
            std::cout << "Set 'foodSpawnedPerSecond' to: " << f_value;
        }

        else if (type == "foodSpawnRadius")
        {
            settings->foodSpawnRadius = f_value;
            std::cout << "Set 'foodSpawnRadius' to: " << f_value;
        }
    }

    else
    {
        CommandUtil::printError("No such setting exists");
    }

}