#include "Console.h"
#include "JsonHandler.h"
#include "OtherCommandHandlers.h"
#include "WaspCommandHandlers.h"
#include "MetaCommandHandlers.h"
#include "StringUtil.h"
#include "CommandUtil.h"
#include "DirectoryHandler.h"
#include "UI.h"
#include <csignal>

#if defined(__linux__) || defined(__APPLE__)
#include <readline/readline.h>
#include <readline/history.h>
#endif

//PRINTS
const std::string initPrint = "Welcome to Eriks Wasp Sim! \nType 'help' to see a list of available commands. \n";
const std::string postCommandPrint = "\n\n$ ";

//COMMANDS JSON
const std::string baseDir = "Assets/Commands/";
const std::string commandsFile = baseDir + "Commands.json";

//COMMAND HANDLERS
CommandHandlerMap mainCommandHandlers =
{
        {"help", MetaCommandHandlers::commandHelp},
        {"syntax", MetaCommandHandlers::commandSyntax},
        {"element", MetaCommandHandlers::commandElement},
        {"wasp", WaspCommandHandlers::commandWasp},
        {"spawn", OtherCommandHandlers::commandSpawn},
        {"kill", OtherCommandHandlers::commandKill},
        {"rset", OtherCommandHandlers::commandResourceSettings}
};

json commands;

const json& Console::getCommands()
{
    return commands;
}

/**
* Safely frees the terminal from readline on linux.
* Without this function the terminal can be blocked from receiving
* input by readline even after the program has exited.
*/
void Console::freeTerminal()
{

#if defined(__linux__) || defined(__APPLE__)
        rl_cleanup_after_signal();
        rl_callback_handler_remove();
        rl_free_line_state();
#endif

        std::cout << std::endl;
    
}

void Console::_init()
{
    //LOAD COMMANDS JSON
    std::string commandsFilePath = DirectoryHandler::appendToProjectRoot(commandsFile);
    commands = JsonHandler::loadJson(commandsFilePath);

    std::cout << initPrint;
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
#if defined(_WIN32) || defined(_WIN64)
        std::cout << postCommandPrint;
        std::getline(std::cin, command);
#else
        char* line = readline(postCommandPrint.c_str());
        if (!line) {exit(0);}

        command = line;
        free(line);

        if (!command.empty()) {add_history(command.c_str());}
#endif

        processCommand(command, mainCommandHandlers);
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