#include "MetaCommandHandlers.h"
#include "Console.h"
#include "JsonHandler.h"
#include "StringUtil.h"
#include "CommandUtil.h"

void MetaCommandHandlers::commandHelp(const std::string& subcommand)
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

void MetaCommandHandlers::commandSyntax(const std::string& subcommand)
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

void MetaCommandHandlers::commandElement(const std::string& subcommand)
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