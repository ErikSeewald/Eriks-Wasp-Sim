#pragma once

#include "Wasp.h"
#include "Food.h"
#include <list>
#include <chrono>
#include <glm/glm.hpp>
#include <vector>
#include <unordered_map>

using Food::FoodEntity;

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
	
	/**
	* Returns the simulations's last delta time
	*/
	std::chrono::duration<double>* getDeltaTime();

	/**
	* Returns the last cached steady_clock::now() call.
	* Updated every tick of the simulation.
	* Useful for having one uniform time_point that all entities on on tick adhere too. 
	* Also faster.
	*/
	std::chrono::steady_clock::time_point* getCachedTimePoint();

	/**
	* Returns a random food entity that has not been eaten. Returns nullptr if no such entity exists.
	*/
	FoodEntity* getRandomAvailableFood();

	/**
	* Synchronous function that locks the food mutex and checks whether the given FoodEntity has been eaten.
	* If so, it returns false. Otherwise it sets food->eaten to true and returns true.
	*/
	bool attemptEatFoodMutex(FoodEntity* food);
}
