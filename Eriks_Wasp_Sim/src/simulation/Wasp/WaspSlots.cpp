#include "WaspSlots.h"

using Strategies::SpawnStrategy;
using Strategies::KillStrategy;

// All wasp objects are created immediately and then kept in memory until the end of the program.
// When wasps are killed they are 'deactivated' and thereby will no longer be updated or rendered.
// Spawning wasps means respawning and thereby 'reactivating' dead wasp objects.
// This way the wasp data can be entirely localized within the wasps vector which means high memory/cache locality
// and better performance.
std::vector<Wasp> wasps(WaspSlots::SLOT_COUNT, Wasp());
int aliveCount = 0;
long deadCount = 0;

std::vector<Wasp>* WaspSlots::getWasps()
{
    return &wasps;
}

/**
* Returns wether there are enough wasp slots available to accommodate waspAddAmount
*/
bool WaspSlots::spaceAvailable(int waspAddAmount)
{
    return aliveCount + waspAddAmount <= SLOT_COUNT;
}

int WaspSlots::getAliveCount()
{
    return aliveCount;
}

long WaspSlots::getDeadCount()
{
    return deadCount;
}

/**
* Spawns the given amount of wasps at the given position using the given SpawnStrategy and spawnRadius.
* Returns true if the wasps have been spawned and false if not enough slots were available.
* Since in reality all slots are already filled with dead wasp objects from the start, when wasps are spawned they
* are actually respawned by resetting that wasp objects attributes and reviving it.
*/
bool WaspSlots::spawnWasps(glm::vec3 position, int amount, SpawnStrategy strategy, float spawnRadius)
{
    if (!spaceAvailable(amount))
    {
        return false;
    }
    int spawnedAmount = 0;

    for (int i = 0; i < WaspSlots::SLOT_COUNT && spawnedAmount < amount; ++i)
    {
        Wasp* wasp = &wasps[i];
        if (wasp->isAlive()) { continue; }
        wasp->respawn();

        if (strategy == SpawnStrategy::RANDOM)
        {
            double x = ((((float)std::rand() / RAND_MAX) * 2.0) - 1.0) * spawnRadius;
            double y = ((((float)std::rand() / RAND_MAX) * 2.0) - 1.0) * spawnRadius;
            double z = ((((float)std::rand() / RAND_MAX) * 2.0) - 1.0) * spawnRadius;
            wasp->position = glm::vec3(position.x + x, position.y + y, position.z + z);
        }

        else
        {
            wasp->position = position;
        }
        spawnedAmount++;
    }
    aliveCount += spawnedAmount;

    return true;
}

/**
* Kills the given amount of wasps using the given KillStrategy.
* Returns the amount of wasps that have been killed. 
* This amount can differ from the desired amount if there weren't enough living wasps to kill.
* Since in reality all slots are always filled with wasp objects, when wasps are killed they
* still remain in memory and are simply no longer updated until they are respawned.
*/
int WaspSlots::killWasps(int amountToKill, KillStrategy strategy)
{
    if (strategy == KillStrategy::ALL)
    {
        amountToKill = -1;
    }

    int killedAmount = 0;
    for (int i = 0; i < WaspSlots::SLOT_COUNT && killedAmount != amountToKill; ++i)
    {
        Wasp* wasp = &wasps[i];
        if (!wasp->isAlive()) { continue; }
        
        wasp->kill();
        killedAmount++;
    }

    aliveCount -= killedAmount;
    deadCount += killedAmount;
    return killedAmount;
}