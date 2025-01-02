#include "WaspCommandHandlers.h"
#include "Console.h"
#include "StringUtil.h"
#include "CommandUtil.h"
#include "UI.h"

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
    if (!StringUtil::isBlank(subcommand))
    {
        CommandUtil::printInvalidSyntaxError();
        return;
    }

    CommandUtil::printSubCommands(waspCommandName);
}

void WaspCommandHandlers::commandWaspKill(const std::string& subcommand)
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
        CommandUtil::printError(CommandUtil::noWaspSelectedPrint);
    }
}

void WaspCommandHandlers::commandWaspSetPos(const std::string& subcommand)
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
    if (selectedWasp != nullptr)
    {
        selectedWasp->setPosition(newPos);
        std::cout << "Wasp's position set to " << newPos.x << "," << newPos.y << "," << newPos.z;
    }
    else
    {
        CommandUtil::printError(CommandUtil::noWaspSelectedPrint);
    }
}

void WaspCommandHandlers::commandWaspSetHp(const std::string& subcommand)
{
    // SELECTED WASP
    Wasp* selectedWasp = UI::getUIState()->selectedWasp;
    if (selectedWasp == nullptr)
    {
        CommandUtil::printError(CommandUtil::noWaspSelectedPrint);
        return;
    }

    //HP STRING
    std::string hpString = StringUtil::getFirstWord(subcommand);
    if (hpString.empty() || !StringUtil::isBlank(StringUtil::cutFirstWord(subcommand)))
    {
        CommandUtil::printInvalidSyntaxError();
        return;
    }

    // SET HP
    int newHp = CommandUtil::convertToInt(hpString);
    int maxHp = selectedWasp->getMaxHP();
    if (newHp < 0 || newHp > maxHp)
    {
        CommandUtil::printError("Hp has to be positive and below the selected wasps max hp");
        return;
    }
    selectedWasp->setHP(newHp);
    std::cout << "Wasp's hp set to " << newHp;
}

void WaspCommandHandlers::commandWaspSetHunger(const std::string& subcommand)
{
    // SELECTED WASP
    Wasp* selectedWasp = UI::getUIState()->selectedWasp;
    if (selectedWasp == nullptr)
    {
        CommandUtil::printError(CommandUtil::noWaspSelectedPrint);
        return;
    }

    //HUNGER STRING
    std::string hungerString = StringUtil::getFirstWord(subcommand);
    if (hungerString.empty() || !StringUtil::isBlank(StringUtil::cutFirstWord(subcommand)))
    {
        CommandUtil::printInvalidSyntaxError();
        return;
    }

    // SET HUNGER SATURATION
    int newSaturation = CommandUtil::convertToInt(hungerString);
    int maxSaturation = selectedWasp->getMaxHungerSaturation();
    if (newSaturation < 0 || newSaturation > maxSaturation)
    {
        CommandUtil::printError("Hunger saturation has to be positive and below the selected wasps max hunger saturation");
        return;
    }
    selectedWasp->setHungerSaturation(newSaturation);
    std::cout << "Wasp's hunger saturation set to " << newSaturation;
}