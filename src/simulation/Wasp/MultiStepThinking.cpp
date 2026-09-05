#include "MultiStepThinking.h"

using MultiStepThinking::ThoughtState;
using MultiStepThinking::ThoughtStepResult;
using Food::FoodEntity;

/**
 * Performs a single step in the multi step process of finding the closest FoodEntity to the given position.
 * Checks CLOSEST_FOOD_CHECKS_PER_ITERATION entities and writes the currently closest entity to the given ThoughtState.
 * If this function is called as the first step in the process, it assumes MultiStepThinking::resetClosestFoodThoughtState was called prior.
 * 
 * Returns the ThoughtStepResult of this iteration. Once that result is FINISHED, the ThoughtState contains the final
 * choice for the closest food entity.
 * 
 * Note: This process technically does not guarantee finding the closest entity as the wasp may move during the steps
 * and get closer to  "ignored" entities of prior steps, though this basically makes no difference in practice.
 */
ThoughtStepResult MultiStepThinking::stepFindClosestFood(glm::vec3 position, ThoughtState* thoughtState)
{
    std::vector<FoodEntity>* foodEntities = Food::getFoodEntities();
    int maxIndex = Food::getMaxIndex();

    int iterations = 0;
    int curIndex = thoughtState->lastCheckedFoodIndex == 0 ? 0 : thoughtState->lastCheckedFoodIndex + 1;
    float curClosestDistance = thoughtState->curClosestFoodDistance;
    FoodEntity* curClosestEntity = thoughtState->curClosestFood;

    while (curIndex <= maxIndex && iterations < MultiStepThinking::CLOSEST_FOOD_CHECKS_PER_ITERATION)
    {
        FoodEntity* entity = &(*foodEntities)[curIndex];   

        if (!entity->eaten)
        {
            float distance = glm::distance(position, entity->position);
            if (distance < curClosestDistance)
            {
                curClosestDistance = distance;
                curClosestEntity = entity;
            }
        }

        curIndex++;
        iterations++;
    }

    // Only update the struct here to reduce heap access in the loop
    thoughtState->lastCheckedFoodIndex = curIndex;
    thoughtState->curClosestFoodDistance = curClosestDistance;
    thoughtState->curClosestFood = curClosestEntity;

    if (curIndex >= maxIndex) { return ThoughtStepResult::FINISHED; }
    return ThoughtStepResult::ONGOING;
}

/**
 * Resets all attributes in the given ThoughtState related to finding the closest food entity.
 */
void MultiStepThinking::resetClosestFoodThoughtState(ThoughtState* thoughtState)
{
    thoughtState->curClosestFood = nullptr;
    thoughtState->curClosestFoodDistance = INFINITY;
    thoughtState->lastCheckedFoodIndex = 0;
}