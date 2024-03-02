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

	void updateWasps();

	void updateDeltaTime();

	/**
	* Initializes everything needed for loop() to run
	*/
	void _loopInit();

	enum class SpawnStrategy
	{
		POINT, RANDOM, INVALID
	};

	bool spawnWasps(glm::vec3 position, int amount, SpawnStrategy strategy);

	enum class KillStrategy
	{
		RANDOM, ALL, INVALID
	};

	int killWasps(int amountToKill, KillStrategy strategy);
	
	/**
	* Returns the simulations's last delta time
	* 
	* @return the simulation's last delta time
	*/
	std::chrono::duration<double>* getDeltaTime();
}