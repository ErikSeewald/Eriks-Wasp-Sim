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
const std::string invalidSyntaxPrint = "Invalid syntax!";
const std::string syntaxNotFoundPrint = "No syntax for this command found";

//COMMANDS JSON
const std::string baseDir = "../../../../../Assets/Commands/";
const std::string baseDirFallback = "../../../Assets/Commands/";
const std::string commandsFile = baseDir + "Commands.json";
const std::string commandsFileFallback = baseDirFallback + "Commands.json";

//COMMAND HANDLERS
CommandHandlerMap mainCommandHandlers =
{
        {"help", CommandHandlers::commandHelp},
        {"syntax", CommandHandlers::commandSyntax},
        {"position", CommandHandlers::commandPosition},
        {"entity", CommandHandlers::commandEntity},
        {"wasp", CommandHandlers::commandWasp},
        {"spawn", CommandHandlers::commandSpawn}
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
    std::string trimmedCommand = trimLeadingWhitespace(command);

    std::string firstWord = getFirstWord(trimmedCommand);
    CommandHandlerMap::iterator iterator = commandHandlers.find(firstWord);
    if (iterator != commandHandlers.end())
    {
        // Execute the command handler function and pass the subcommand after firstWord to it
        iterator->second(trimmedCommand.substr(firstWord.length()));
    }
    else
    {
        printInvalidSyntaxError();
    }
}

/**
* Returns the first word in the given string or an empty string if no word is found.
* A word is defined as a string of characters separated by whitespace.
* 
* @param str the string to get the first word from
* @return The first word in the given string or an empty string
*/
std::string Console::getFirstWord(const std::string& str)
{
    std::istringstream iss(str + " "); //add " ". Otherwise it wont recognize strings that are made up of one word
    std::string word;
    iss >> word; //extracts chars from iss and stores them in word until a whitespace char is encountered
    return word;
}

std::string Console::trimLeadingWhitespace(const std::string& str)
{
    for (int i = 0; i < str.length(); i++)
    {
        if (!std::isspace(str[i]))
        {
            return str.substr(i);
        }
    }
    return std::string();
}

bool Console::isBlank(const std::string& str)
{
    return trimLeadingWhitespace(str).empty();
}

/**
* Prints the name and description of the given command.
* Leads to a runtime error if the given json is invalid.
*
* @param command the command to print
*/
void Console::printCommandDescription(const json& command)
{
    // extract json values like this because '<<' puts quotation marks around normal json values
    std::string name = command["name"].get<std::string>();
    std::string description = command["description"].get<std::string>();

    std::cout << "\n " << name;

    int tabCount = 4 - (name.length() / 4);
    for (int i = 0; i < tabCount; i++)
    {
        std::cout << "\t";
    }

    std::cout << description << std::endl;
}

/**
* Prints the syntax of the given command.
* Leads to a runtime error if the given json is invalid.
*
* @param command the command to print the syntax of
*/
void Console::printCommandSyntax(const json& command)
{
    static const std::string syntaxKey = "syntax";
    if (!JsonHandler::hasKey(command, syntaxKey))
    {
        std::cout << syntaxNotFoundPrint << std::endl;
        return;
    }

    std::string syntax = command[syntaxKey].get<std::string>();
    std::cout << syntax << std::endl;
}

/**
* Prints all subcommands of the parent command, provided it can be found.
*
* @param parentCommandName the 'name' value of the parent command
*/
void Console::printSubCommands(const json& parentCommandName)
{
   static const std::string subcommandsKey = "subcommands";

    const json& command = JsonHandler::findByName(commands, parentCommandName);
    if (!JsonHandler::hasKey(command, subcommandsKey))
    {
        return;
    }

    for (const json& subcommand : command[subcommandsKey])
    {
        Console::printCommandDescription(subcommand);
    }
}

void Console::printInvalidSyntaxError()
{
    std::cout << invalidSyntaxPrint;
}