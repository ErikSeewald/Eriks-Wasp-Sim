#include "DirectoryHandler.h"
#include <filesystem>
#include <iostream>

#ifdef _WIN32
static constexpr char PATH_SEPARATOR = '\\';
#else
static constexpr char PATH_SEPARATOR = '/';
#endif

namespace fs = std::filesystem;

const std::string ROOT_NAME = "Eriks-Wasp-Sim";
std::string rootPath = "Not_initialized";


/**
* Initializes the DirectoryHandler. Finds and tracks the project's root directory.
*/
void DirectoryHandler::init()
{
    fs::path startingPath = fs::current_path();
    fs::path currentPath = startingPath;

    // Try to find the root directory by walking upward
    while (true)
    {
        if (currentPath.filename() == ROOT_NAME)
        {
            rootPath = normalizePathDividers(currentPath.string());
            return;
        }

        fs::path parent = currentPath.parent_path();

        if (parent == currentPath)  // Reached the filesystem root
        {
            break;
        }

        currentPath = parent;
    }

    // Upward search failed, try recursive downward search from original path
    for (auto& p : fs::recursive_directory_iterator(startingPath))
    {
        if (p.is_directory() && p.path().filename() == ROOT_NAME)
        {
            rootPath = normalizePathDividers(p.path().string());
            return;
        }
    }

    std::cerr << "Failed to find project source directory '" << ROOT_NAME << "'" << std::endl;
    exit(EXIT_FAILURE);
}

/**
* Returns the absolute path to the root of the project.
* Assumes DirectoryHandler::init() has been called prior.
*/
const std::string& DirectoryHandler::getProjectRootPath()
{
    return rootPath;
}

/**
* Appends the given string to the absolute path to the root of the project and returns it.
* Assumes DirectoryHandler::init() has been called prior.
*/
std::string DirectoryHandler::appendToProjectRoot(const std::string& toAppend)
{
    std::string normalizedToAppend = normalizePathDividers(toAppend);

    if (!normalizedToAppend.empty() && normalizedToAppend.front() == PATH_SEPARATOR) {
        normalizedToAppend.erase(normalizedToAppend.begin());
    }

    if (rootPath.back() != PATH_SEPARATOR) {
        return rootPath + PATH_SEPARATOR + normalizedToAppend;
    }

    return rootPath + normalizedToAppend;
}

/**
* Normalizes the given path string to the system specific path dividers of the current runtime.
*/
std::string DirectoryHandler::normalizePathDividers(const std::string& path)
{
    std::string result = path;
    #ifdef _WIN32
        for (char& c : result) {
            if (c == '/') {
                c = '\\';
            }
        }
    #else
        for (char& c : result) {
            if (c == '\\') {
                c = '/';
            }
        }
    #endif
    return result;
}