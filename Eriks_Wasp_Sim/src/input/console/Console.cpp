#include "Console.h"
#include "JsonHandler.h"
#include "OtherCommandHandlers.h"
#include "WaspCommandHandlers.h"
#include "MetaCommandHandlers.h"
#include "StringUtil.h"
#include "CommandUtil.h"

//PRINTS
const std::string initPrint = "Welcome to Eriks Wasp Sim! \nType 'help' to see a list of available commands. \n";
const std::string postCommandPrint = "\n\n$ ";

//COMMANDS JSON
const std::string baseDir = "../../../../../Assets/Commands/";
const std::string baseDirFallback = "../../../Assets/Commands/";
const std::string commandsFile = baseDir + "Commands.json";
const std::string commandsFileFallback = baseDirFallback + "Commands.json";

//COMMAND HANDLERS
CommandHandlerMap mainCommandHandlers =
{
        {"help", MetaCommandHandlers::commandHelp},
        {"syntax", MetaCommandHandlers::commandSyntax},
        {"element", MetaCommandHandlers::commandElement},
        {"wasp", WaspCommandHandlers::commandWasp},
        {"spawn", OtherCommandHandlers::commandSpawn},
        {"kill", OtherCommandHandlers::commandKill}
};

json commands;

const json& Console::getCommands()
{
    return commands;
}

void Console::_init()
{
    //LOAD COMMANDS JSON
    try {
        commands = JsonHandler::loadJson(commandsFile);
    }
    catch (const std::runtime_error& e)
    {
        //FALLBACK FILE
        try {
            commands = JsonHandler::loadJson(commandsFileFallback);
        }
        catch (const std::runtime_error& e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
            exit(1);
        }
    }

    std::cout << initPrint << postCommandPrint;
}

/**
* Initializes the console and starts the loop needed to get continuous input from the user.
*/
void Console::startLoop()
{
    _init();

    std::string command;
    while (true) 
    {
        std::getline(std::cin, command);
        processCommand(command, mainCommandHandlers);
        std::cout << postCommandPrint;
    }
}

/**
* Tries to process the given subcommand by finding a fitting command handler in the CommandHandlerMap
* of the subcommands parent command.
* Prints an error message if no fitting command handler is found.
*
* @param subcommand the subcommand to process
* @param commandHandlers the CommandHandlerMap for the parent command
*/
void Console::processCommand(const std::string& command, CommandHandlerMap& commandHandlers)
{
    std::string trimmedCommand = StringUtil::trimLeadingWhitespace(command);

    std::string firstWord = StringUtil::getFirstWord(trimmedCommand);
    CommandHandlerMap::iterator iterator = commandHandlers.find(firstWord);
    if (iterator != commandHandlers.end())
    {
        // Execute the command handler function and pass the subcommand after firstWord to it
        iterator->second(trimmedCommand.substr(firstWord.length()));
    }
    else
    {
        CommandUtil::printInvalidSyntaxError();
    }
}

void Console::sendDebugMessage(const std::string& message)
{
    std::cout << "\r" << std::string(80, ' ') << "\r"; // clear current line
    std::cout << message << std::endl;
    std::cout << postCommandPrint; // back to normal state
}