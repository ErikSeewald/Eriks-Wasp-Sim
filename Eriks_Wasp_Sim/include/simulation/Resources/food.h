#pragma once

#include "EntitySlots.h"
#include "Strategies.h"
#include <glm/glm.hpp>

using EntitySlots::EntitySlot;
using Strategies::SpawnStrategy;
using Strategies::KillStrategy;

/**
* @namespace Food
* @brief A namespace for all code related to food entities.
*/
namespace Food
{
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

	/**
	* Allocates space in the food slot linked list and creates a slot for the given food.
	*
	* @param food the food to allocate a slot for
	*/
	void allocateFoodSlot(FoodEntity* food);

	void removeFoodSlot(EntitySlot* foodSlot);

	EntitySlot* getFoodSlots();

	bool spaceAvailable(int foodAddAmount);

	bool spawnFood(glm::vec3 position, int amount, SpawnStrategy strategy, float spawnRadius);

	int killFood(int amountToKill, KillStrategy strategy);
}