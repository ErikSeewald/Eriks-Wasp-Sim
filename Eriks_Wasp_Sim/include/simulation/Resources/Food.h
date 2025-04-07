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
	* Returns an index that can be used as the upper bound for loops.
	* After this index there are no more uneaten food entities in the list.
	* This optimization relies heavily on well managed calls to updateMaxIndex()
	* and on killing entities with higher indices before entities with smaller indices.
	*/
	int getMaxIndex();

	/**
	* Updates maxIndex by finding the largest index containing an uneaten food entity in the entity vector.
	*/
	void updateMaxIndex();

	/**
	* Returns wether there are enough food slots available to accommodate foodAddAmount.
	*/
	bool spaceAvailable(int foodAddAmount);

	/**
	* Used to register that a food entity has been eaten thereby update uneatenFoodCount.
	*/
	void registerEntityEaten();

	bool spawnFood(glm::vec3 position, int amount, SpawnStrategy strategy, float spawnRadius);

	int killFood(int amountToKill, KillStrategy strategy);
}