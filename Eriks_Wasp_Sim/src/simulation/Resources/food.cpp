#include "Food.h"

using Food::FoodEntity;

std::vector<FoodEntity> foodEntities(Food::SLOT_COUNT, {0, glm::vec3(0,0,0), true});

std::vector<FoodEntity>* Food::getFoodEntities()
{
	return &foodEntities;
}

/**
* Returns wether there are enough food slots available to accommodate foodAddAmount
*/
bool Food::spaceAvailable(int foodAddAmount)
{
    int foodCount = 0;
    for (int i = 0; i < Food::SLOT_COUNT; ++i)
    {
        FoodEntity* food = &foodEntities[i];
        if (!food->eaten) { foodCount++; }
    }
	return foodCount + foodAddAmount <= Food::SLOT_COUNT;
}

bool Food::spawnFood(glm::vec3 position, int amount, SpawnStrategy strategy, float spawnRadius)
{
    if (!spaceAvailable(amount))
    {
        return false;
    }
    int spawnedAmount = 0;

    for (int i = 0; i < Food::SLOT_COUNT && spawnedAmount < amount; ++i)
    {
        FoodEntity* food = &foodEntities[i];
        if (!food->eaten) { continue; }

        if (strategy == SpawnStrategy::RANDOM)
        {
            double x = ((((float)std::rand() / RAND_MAX) * 2.0) - 1.0) * spawnRadius;
            double y = ((((float)std::rand() / RAND_MAX) * 2.0) - 1.0) * spawnRadius;
            double z = ((((float)std::rand() / RAND_MAX) * 2.0) - 1.0) * spawnRadius;
            food->position = glm::vec3(position.x + x, position.y + y, position.z + z);
            food->hungerPoints = 10;
        }

        else
        {
            food->position = position;
        }

        food->eaten = false;
        spawnedAmount++;
    }

    return true;
}

int Food::killFood(int amountToKill, KillStrategy strategy)
{
    if (strategy == KillStrategy::ALL)
    {
        amountToKill = -1;
    }

    int killedAmount = 0;

    for (int i = 0; i < Food::SLOT_COUNT && killedAmount < amountToKill; ++i)
    {
        FoodEntity* food = &foodEntities[i];
        if (food->eaten) { continue; }

        food->eaten = true;
        killedAmount++;
    }

    return killedAmount;
}