#include "Food.h"
#include "MemoryManager.h"

EntitySlot* foodSlotsStart;
EntitySlot* foodSlotsEnd;
int foodCount = 0;
const int MAX_FOOD_COUNT = 100000;

/**
* Allocates space in the food slot linked list and creates a slot for the given food.
*
* @param food the food to allocate a slot for
*/
void Food::allocateFoodSlot(FoodEntity* food)
{
	EntitySlots::allocateSlot(food, &foodSlotsStart, &foodSlotsEnd);
	foodCount++;
}

void Food::removeFoodSlot(EntitySlot* foodSlot)
{
	EntitySlots::removeSlot(foodSlot, &foodSlotsStart, &foodSlotsEnd);
	foodCount--;
}

EntitySlot* Food::getFoodSlots()
{
	return foodSlotsStart;
}

bool Food::spaceAvailable(int foodAddAmount)
{
	return foodCount + foodAddAmount <= MAX_FOOD_COUNT;
}

bool Food::spawnFood(glm::vec3 position, int amount, SpawnStrategy strategy, float spawnRadius)
{
    if (!spaceAvailable(amount))
    {
        return false;
    }

    if (strategy == SpawnStrategy::RANDOM)
    {
        for (int i = 0; i < amount; i++)
        {
            FoodEntity* food = new FoodEntity();

            double x = ((((float)std::rand() / RAND_MAX) * 2.0) - 1.0) * spawnRadius;
            double y = ((((float)std::rand() / RAND_MAX) * 2.0) - 1.0) * spawnRadius;
            double z = ((((float)std::rand() / RAND_MAX) * 2.0) - 1.0) * spawnRadius;
            food->position = glm::vec3(position.x + x, position.y + y, position.z + z);

            allocateFoodSlot(food);
        }
    }

    else
    {
        for (int i = 0; i < amount; i++)
        {
            FoodEntity* food = new FoodEntity();
            food->position = position;
            allocateFoodSlot(food);
        }
    }

    return amount >= 1;
}

int Food::killFood(int amountToKill, KillStrategy strategy)
{
    if (strategy == KillStrategy::ALL)
    {
        amountToKill = -1;
    }

    int killedAmount = 0;
    EntitySlot* foodSlot = getFoodSlots();
    while (foodSlot != nullptr && killedAmount != amountToKill)
    {
        FoodEntity* food = (FoodEntity*) foodSlot->entity;
        food->eaten = true;
        removeFoodSlot(foodSlot);
        killedAmount++;
        foodSlot = foodSlot->next;
    }

    MemoryManager::scheduleCleanup();
    return killedAmount;
}