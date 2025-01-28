#pragma once

#include "Wasp.h"
#include "food.h"
#include <list>
#include <chrono>
#include <glm/glm.hpp>
#include <vector>

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
	* 
	* @return the simulation's last delta time
	*/
	std::chrono::duration<double>* getDeltaTime();

	FoodEntity* getFirstFoodInApproxRadius(glm::vec3 position, float radius);

	/**
	* Synchronous function that locks the food mutex and checks whether the given FoodEntity has been eaten.
	* If so, it returns false. Otherwise it sets food->eaten to true and returns true.
	*/
	bool attemptEatFoodMutex(FoodEntity* food);
}