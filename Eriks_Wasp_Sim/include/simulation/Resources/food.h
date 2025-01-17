#pragma once

#include "Strategies.h"
#include <vector>
#include <glm/glm.hpp>

using Strategies::SpawnStrategy;
using Strategies::KillStrategy;

/**
* @namespace Food
* @brief A namespace for code related to food entities.
*/
namespace Food
{
	// Follows the same slot pattern as WaspSlots.h
	// For more info read the docs (SLOT_COUNT, spawnWasps, killWasps) over there
	const static int SLOT_COUNT = 1000;

	/**
	* @struct Food
	* @brief A struct representing a single food entity in the simulation.
	*/
	struct FoodEntity
	{
		int hungerPoints;
		glm::vec3 position;
		bool eaten;
	};

	std::vector<FoodEntity>* getFoodEntities();

	/**
	* Returns wether there are enough food slots available to accommodate foodAddAmount
	*/
	bool spaceAvailable(int foodAddAmount);

	bool spawnFood(glm::vec3 position, int amount, SpawnStrategy strategy, float spawnRadius);

	int killFood(int amountToKill, KillStrategy strategy);
}