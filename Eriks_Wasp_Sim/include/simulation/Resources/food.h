#pragma once

#include "EntitySlots.h"
#include <glm/glm.hpp>

using EntitySlots::EntitySlot;

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
}