#include "Console.h"
#include "JsonHandler.h"
#include "CommandHandlers.h"
#include <string>
#include <sstream>
#include "Simulation.h"
#include <glm/glm.hpp>

//PRINTS
const std::string initPrint = "Welcome to Eriks Wasp Sim! \nType 'help' to see a list of available commands. \n";
const std::string postCommandPrint = "\n\n$ ";
const std::string invalidCommandPrint = "Command not found";

//COMMANDS JSON
const std::string baseDir = "../../../../../Assets/Commands/";
const std::string baseDirFallback = "../../../Assets/Commands/";
const std::string commandsFile = baseDir + "Commands.json";
const std::string commandsFileFallback = baseDirFallback + "Commands.json";

//COMMAND HANDLERS
using CommandHandler = std::function<void()>;
using CommandHandlerMap = std::unordered_map<std::string, CommandHandler>;
CommandHandlerMap commandHandlers =
{
        {"help", CommandHandlers::commandHelp}
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
        processCommand(command);
        std::cout << postCommandPrint;
    }
}

/**
* Tries to process the given command by finding a fitting command handler.
* Prints an error message if no fitting command handler is found.
* 
* @param command the command to process
*/
void Console::processCommand(const std::string& command)
{
    CommandHandlerMap::iterator iterator = commandHandlers.find(_getFirstWord(command));
    if (iterator != commandHandlers.end())
    {
        iterator->second(); // Execute the command handler function
    }
    else
    {
        std::cout << invalidCommandPrint;
    }
}

/**
* Returns the first word in the given string or an empty string if no word is found.
* A word is defined as a string of characters separated by whitespace.
* 
* @param str the string to get the first word from
* @return The first word in the given string or an empty string
*/
std::string Console::_getFirstWord(const std::string& str)
{
    std::istringstream iss(str);
    std::string word;
    iss >> word; //extracts chars from iss and stores them in word until a whitespace char is encountered
    return word;
}
