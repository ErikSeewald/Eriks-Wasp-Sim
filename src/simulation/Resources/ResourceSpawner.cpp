#include "ResourceSpawner.h"
#include "RNG.h"

namespace ResourceSpawner
{
    ResourceSettings settings;

    ResourceSettings* getResourceSettings()
    {
        return &settings;
    }

    /**
    * Spawns new global resources based on the given delta time and current ResourceSettings.
    */
    void update(std::chrono::duration<double>* deltaTime)
    {
        updateFood(deltaTime);
    }


    void updateFood(std::chrono::duration<double>* deltaTime)
    {
        static double timeAccumulator = 0.0;
        static double amountAccumulator = 0.0;

        timeAccumulator += deltaTime->count();

        if (timeAccumulator >= 1.0)
        {
            amountAccumulator += timeAccumulator * settings.foodSpawnedPerSecond;
            int amountToSpawn = (int) amountAccumulator;
            
            if (amountToSpawn > 0)
            {
                amountAccumulator -= amountToSpawn;

                float radius = settings.foodSpawnRadius;
                Food::spawnFood(glm::vec3(0, 0, 0), amountToSpawn, Strategies::SpawnStrategy::RANDOM, radius);
            }

            timeAccumulator = 0.0;
        }
    }
}