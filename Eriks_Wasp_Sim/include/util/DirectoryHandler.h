#pragma once

#include <string>


/**
* @namespace DirectoryHandler
* @brief Namespace for os-independent directory handling.
*/
namespace DirectoryHandler
{
	/**
	* Initializes the DirectoryHandler. Finds and tracks the project's root directory.
	*/
	void init();

	/**
	* Returns the absolute path to the root of the project.
	* Assumes DirectoryHandler::init() has been called prior.
	*/
	const std::string& getProjectRootPath();

	/**
	* Appends the given string to the absolute path to the root of the project and returns it.
	* Assumes DirectoryHandler::init() has been called prior.
	*/
	std::string appendToProjectRoot(const std::string& toAppend);

	/**
	* Normalizes the given path string to the system specific path dividers of the current runtime.
	*/
	std::string normalizePathDividers(const std::string& path);
}