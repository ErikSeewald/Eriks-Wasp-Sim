#pragma once

#include "Wasp.h"
#include <list>
#include <chrono>
#include <glm/glm.hpp>

/**
* @namespace Simulation
* @brief Namespace for the main control loop of the simulation.
*/
namespace Simulation 
{
	/**
	* Starts and runs the simulation loop.
	*/
	void startLoop();

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

	bool spawnWasps(glm::vec3 position, int amount);
	
	/**
	* Returns the simulations's last delta time
	* 
	* @return the simulation's last delta time
	*/
	std::chrono::duration<double>* getDeltaTime();
}