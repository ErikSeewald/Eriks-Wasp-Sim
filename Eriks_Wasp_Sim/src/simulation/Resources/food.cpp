#include "Food.h"
#include <cstdlib>

using Food::FoodEntity;

// Follows the same slot pattern as WaspSlots.h
// For more info read the docs (SLOT_COUNT, spawnWasps, killWasps) over there
const static int SLOT_COUNT = 10000;

int food_maxIndex = 0;
int uneatenFoodCount = 0;

std::vector<FoodEntity> foodEntities(SLOT_COUNT, {0, glm::vec3(0,0,0), true});

std::vector<FoodEntity>* Food::getFoodEntities()
{
	return &foodEntities;
}

/**
* Returns an index that can be used as the upper bound for loops.
* After this index there are no more uneaten food entities in the list.
* This optimization relies heavily on well managed calls to updateMaxIndex()
* and on killing entities with higher indices before entities with smaller indices.
*/
int Food::getMaxIndex()
{
    return food_maxIndex;
}

/**
* Updates maxIndex by finding the largest index containing an uneaten food entity in the entity vector.
*/
void Food::updateMaxIndex()
{
    int index = -1;
    for (int i = 0; i < SLOT_COUNT; ++i)
    {
        FoodEntity* food = &foodEntities[i];
        if (!food->eaten) { index = i; }
    }
    food_maxIndex = index >= 0 ? index + 1 : 0; // +1 here to make using it for loops easier
}

/**
* Returns wether there are enough food slots available to accommodate foodAddAmount.
*/
bool Food::spaceAvailable(int foodAddAmount)
{
    return uneatenFoodCount + foodAddAmount <= SLOT_COUNT;
}

/**
* Used to register that a food entity has been eaten thereby update uneatenFoodCount.
*/
void Food::registerEntityEaten()
{
    uneatenFoodCount--;
}

bool Food::spawnFood(glm::vec3 position, int amount, SpawnStrategy strategy, float spawnRadius)
{
    if (!spaceAvailable(amount))
    {
        return false;
    }
    int spawnedAmount = 0;

    for (int i = 0; i < SLOT_COUNT && spawnedAmount < amount; ++i)
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

    uneatenFoodCount += spawnedAmount;
    updateMaxIndex();
    return true;
}

int Food::killFood(int amountToKill, KillStrategy strategy)
{
    if (strategy == KillStrategy::ALL)
    {
        amountToKill = -1;
    }
    int killedAmount = 0;


    // Note: Even though the default KillStrategy is named 'RANDOM', in reality food is
    // always killed based on its index starting from the back of the vector. This ensures
    // that food_maxIndex is always as small as possible when using the kill function.
    // When using a random SpawnStrategy this index based approach still looks random.
    for (int i = SLOT_COUNT - 1; i >= 0 && killedAmount != amountToKill; --i)
    {
        FoodEntity* food = &foodEntities[i];
        if (food->eaten) { continue; }

        food->eaten = true;
        killedAmount++;
    }

    uneatenFoodCount -= killedAmount;
    updateMaxIndex();
    return killedAmount;
}