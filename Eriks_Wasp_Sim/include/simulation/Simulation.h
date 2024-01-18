/**
 * @file Simulation.h
 * @brief This file serves as the header for Simulation.cpp.
 */

#pragma once

#include "Wasp.h"
#include <list>
#include <chrono>

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
	* Initializes everything needed for loop() to run
	*/
	void _loopInit();

	/**
	* Returns a list of all active wasps in the simulation.
	* 
	* @return list of all active wasps in the simulation.
	*/
	std::list<Wasp*>* getWasps();
	
	/**
	* Returns the simulations's last delta time
	* 
	* @return the simulation's last delta time
	*/
	std::chrono::duration<double>* getDeltaTime();
}