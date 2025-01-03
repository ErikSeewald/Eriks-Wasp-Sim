#pragma once

#include "food.h"
#include <chrono>

/**
* @namespace ResourceSpawner
* @brief A namespace for all code related to spawning resource entities.
*/
namespace ResourceSpawner
{
	/**
	* @struct ResourceSettings
	* @brief Struct to hold and pass around key values describing the current settings used by the resource spawner.
	*/
	struct ResourceSettings
	{
		float foodSpawnedPerSecond = 1;
		float foodSpawnRadius = 20; 
	};

	ResourceSettings* getResourceSettings();

	/**
	* Spawns new global resources based on the given delta time and current ResourceSettings.
	*/
	void update(std::chrono::duration<double>* deltaTime);

	void updateFood(std::chrono::duration<double>* deltaTime);
}