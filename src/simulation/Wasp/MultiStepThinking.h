#pragma once

#include "Food.h"

/**
* @namespace MultiStepThinking
* @brief Namespace for computations ("thoughts") performed by wasps over multiple iterations of the simulation update loop.
*/
namespace MultiStepThinking
{
    // By limiting the amount of food entities that are checked per iteration and instead finding the closest entity
    // over a period of time, the performance impact of thousands of wasps checking thousands of food entities all the
    // time can be mitigated.
    // This maximum, along with the maximum amount of wasp entities, gives a much more reasonable upper ceiling for 
    // the number of calculations performed at a time.
    constexpr int CLOSEST_FOOD_CHECKS_PER_ITERATION = 50;

    /**
     * @struct ThoughtState
     * @brief Struct representing the current state of the thoughts/computations of a wasp that span multiple update iterations.
     */
    struct ThoughtState
    {
        // The closest food entity that the wasp has found so far.
        Food::FoodEntity* curClosestFood = nullptr;
        float curClosestFoodDistance = INFINITY;
        int lastCheckedFoodIndex = 0;
    };

    /**
     * @enum ThoughtStepResult
     * @brief Enum representing the result of a single step in a multi step thought process.
     */
    enum ThoughtStepResult
    {
        FINISHED, // The thought process has finished and the result is available in the ThoughtState struct
        ONGOING, // The thought proccess is still going on and the ThoughtState struct only contains intermediary results
    };

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
    ThoughtStepResult stepFindClosestFood(glm::vec3 position, ThoughtState* thoughtState);

    /**
     * Resets all attributes in the given ThoughtState related to finding the closest food entity.
     */
    void resetClosestFoodThoughtState(ThoughtState* thoughtState);
}