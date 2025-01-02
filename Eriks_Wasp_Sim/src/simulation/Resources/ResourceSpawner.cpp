#include "ResourceSpawner.h"
#include "RNG.h"

using ResourceSpawner::ResourceSettings;

ResourceSettings settings;

ResourceSettings* ResourceSpawner::getResourceSettings()
{
    return &settings;
}

/**
* Spawns new global resources based on the given delta time and current ResourceSettings.
*/
void ResourceSpawner::update(std::chrono::duration<double>* deltaTime)
{
    ResourceSpawner::updateFood(deltaTime);
}


#include <iostream>
void ResourceSpawner::updateFood(std::chrono::duration<double>* deltaTime)
{
    static double foodTimeAccumulator = 0.0;
    foodTimeAccumulator += deltaTime->count();

    if (foodTimeAccumulator >= 1.0)
    {
        int amountToSpawn = foodTimeAccumulator * settings.foodSpawnedPerSecond;
        foodTimeAccumulator = 0.0;

        float radius = settings.foodSpawnRadius;
        float halfRadius = radius / 2;
        Food::spawnFood(glm::vec3(0,0,0), amountToSpawn, Strategies::SpawnStrategy::RANDOM, radius);
    }
}