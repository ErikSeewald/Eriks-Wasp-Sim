#include "WaspCommandHandlers.h"
#include "Console.h"
#include "StringUtil.h"
#include "CommandUtil.h"
#include "UI.h"
#include "WaspSlots.h"

static const std::string waspCommandName = "wasp";
CommandHandlerMap waspCommandHandlers =
{
        {"help", WaspCommandHandlers::commandWaspHelp},
        {"kill", WaspCommandHandlers::commandWaspKill},
        {"setpos", WaspCommandHandlers::commandWaspSetPos},
        {"sethp", WaspCommandHandlers::commandWaspSetHp},
        {"sethunger", WaspCommandHandlers::commandWaspSetHunger}
};

void WaspCommandHandlers::commandWasp(const std::string& subcommand)
{
    Console::processCommand(subcommand, waspCommandHandlers);
}

void WaspCommandHandlers::commandWaspHelp(const std::string& subcommand)
{
    if (!StringUtil::isBlank(subcommand) && StringUtil::trimLeadingWhitespace(subcommand) != "all")
    {
        CommandUtil::printInvalidSyntaxError();
        return;
    }

    CommandUtil::printSubCommands(waspCommandName);
}

#include "OtherCommandHandlers.h"
void WaspCommandHandlers::commandWaspKill(const std::string& subcommand)
{
    static const std::string successfulKillPrint = "Wasp killed successfully!";
    if (StringUtil::getFirstWord(subcommand) == "all")
    { 
        OtherCommandHandlers::commandKill("wasp all" ); 
        return; 
    }

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
        CommandUtil::printError(CommandUtil::noWaspSelectedPrint);
    }
}

void WaspCommandHandlers::commandWaspSetPos(const std::string& subcommand)
{
    //POSITION STRING
    std::string posString = StringUtil::getFirstWord(subcommand);

    std::string cutCommand = StringUtil::cutFirstWord(subcommand);
    bool allWasps = StringUtil::getFirstWord(cutCommand) == "all";

    if (posString.empty() || !StringUtil::isBlank(StringUtil::cutFirstWord(cutCommand)))
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
    if (allWasps)
    {
        std::vector<Wasp>* wasps = WaspSlots::getWasps();
        for (int i = 0; i < WaspSlots::SLOT_COUNT; ++i)
        {   
            Wasp* wasp = &(*wasps)[i];
            if (!wasp->isAlive()) { continue; }

            wasp->position = newPos;
        }
        std::cout << "All wasp positions set to " << newPos.x << "," << newPos.y << "," << newPos.z;
    }

    else
    {
        Wasp* selectedWasp = UI::getUIState()->selectedWasp;
        if (selectedWasp != nullptr)
        {
            selectedWasp->position = newPos;
            std::cout << "Wasp's position set to " << newPos.x << "," << newPos.y << "," << newPos.z;
        }
        else
        {
            CommandUtil::printError(CommandUtil::noWaspSelectedPrint);
        }
    }
}

void WaspCommandHandlers::commandWaspSetHp(const std::string& subcommand)
{
    //HP STRING
    std::string hpString = StringUtil::getFirstWord(subcommand);

    std::string cutCommand = StringUtil::cutFirstWord(subcommand);
    bool allWasps = StringUtil::getFirstWord(cutCommand) == "all";

    if (hpString.empty() || !StringUtil::isBlank(StringUtil::cutFirstWord(cutCommand)))
    {
        CommandUtil::printInvalidSyntaxError();
        return;
    }

    // SET HP
    int newHp = CommandUtil::convertToInt(hpString);
    if (newHp < 0)
    {
        CommandUtil::printError("Hp cannot be < 0");
        return;
    }

    if (allWasps)
    {
        std::vector<Wasp>* wasps = WaspSlots::getWasps();
        for (int i = 0; i < WaspSlots::SLOT_COUNT; ++i)
        {
            Wasp* wasp = &(*wasps)[i];
            if (!wasp->isAlive()) { continue; }

            int maxHp = wasp->getMaxHP();
            int curNewHp = newHp;
            if (newHp > maxHp)
            {
                curNewHp = maxHp;
            }
            wasp->setHP(curNewHp);
        }
        std::cout << "All wasp's hp set to min(" << newHp << ", their maximum hp)";
    }

    else
    {
        Wasp* selectedWasp = UI::getUIState()->selectedWasp;
        if (selectedWasp == nullptr)
        {
            CommandUtil::printError(CommandUtil::noWaspSelectedPrint);
            return;
        }

        int maxHp = selectedWasp->getMaxHP();
        if (newHp > maxHp)
        {
            std::cout << "Desired hp had to be limited to the wasp's max hp" << std::endl;
            newHp = maxHp;
        }
        selectedWasp->setHP(newHp);
        std::cout << "Wasp's hp set to " << newHp;
    }
}

void WaspCommandHandlers::commandWaspSetHunger(const std::string& subcommand)
{
    //HUNGER STRING
    std::string hungerString = StringUtil::getFirstWord(subcommand);

    std::string cutCommand = StringUtil::cutFirstWord(subcommand);
    bool allWasps = StringUtil::getFirstWord(cutCommand) == "all";

    if (hungerString.empty() || !StringUtil::isBlank(StringUtil::cutFirstWord(cutCommand)))
    {
        CommandUtil::printInvalidSyntaxError();
        return;
    }

    // SET HUNGER SATURATION
    int newSaturation = CommandUtil::convertToInt(hungerString);

    if (newSaturation < 0)
    {
        CommandUtil::printError("Hunger saturation cannot be < 0");
        return;
    }

    if (allWasps)
    {
        std::vector<Wasp>* wasps = WaspSlots::getWasps();
        for (int i = 0; i < WaspSlots::SLOT_COUNT; ++i)
        {
            Wasp* wasp = &(*wasps)[i];
            if (!wasp->isAlive()) { continue; }

            int maxSaturation = wasp->getMaxHungerSaturation();
            int curNewSaturation = newSaturation;
            if (newSaturation > maxSaturation)
            {
                curNewSaturation = maxSaturation;
            }
            wasp->setHungerSaturation(curNewSaturation);
        }
        std::cout << "All wasp's hunger saturation set to min(" << newSaturation << ", their maximum hunger saturation)";
    }

    else
    {
        Wasp* selectedWasp = UI::getUIState()->selectedWasp;
        if (selectedWasp == nullptr)
        {
            CommandUtil::printError(CommandUtil::noWaspSelectedPrint);
            return;
        }

        int maxSaturation = selectedWasp->getMaxHungerSaturation();
        if (newSaturation > maxSaturation)
        {
            std::cout << "Desired hunger saturation had to be limited to the wasp's max hunger saturation" << std::endl;
            newSaturation = maxSaturation;
        }
        selectedWasp->setHungerSaturation(newSaturation);
        std::cout << "Wasp's hunger saturation set to " << newSaturation;
    }
}