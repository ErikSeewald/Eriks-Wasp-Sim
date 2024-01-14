/**
 * @file Simulation.h
 * @brief This file serves as the header for Simulation.cpp.
 */

#include "Wasp.h"
#include <list>

/**
* @namespace Simulation
* @brief Namespace for the main control loop of the simulation.
*/
namespace Simulation 
{
	/**
	* Starts and runs the simulation loop.
	*/
	void loop();

	/**
	* Returns a list of all active wasps in the simulation.
	* 
	* @return list of all active wasps in the simulation.
	*/
	std::list<Wasp*> getWasps();
}