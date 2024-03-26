#include "Food.h"

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