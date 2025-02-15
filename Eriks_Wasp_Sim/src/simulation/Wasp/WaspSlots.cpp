#include "WaspSlots.h"

using Strategies::SpawnStrategy;
using Strategies::KillStrategy;

// Total number of wasp slots
const static int SLOT_COUNT = 100000;

int wasp_maxIndex = 0; // Tracks the highest index containing a living wasp to optimize loops
int aliveCount = 0; // Number of currently living wasps
long deadCount = 0; // Total number of deaths

// All wasp objects are created immediately and then kept in memory until the end of the program.
// When wasps are killed they are 'deactivated' and thereby will no longer be updated or rendered.
// Spawning wasps means respawning and thereby 'reactivating' dead wasp objects.
// This way the wasp data can be entirely localized within the wasps vector which means high memory/cache locality
// and better performance.
// 
// Note that it is still better to use a vector on the heap instead of an array despite the fixed size
// because having this much data constantly stored on the stack and accessed through it introduces stutter.
// Since the vector only grows at the start of the program and the memory stays locally fixed in one 
// continuous block on the heap afterwards, the initialization overhead can be ignored.
// (At SLOT_COUNT > 100000 an array can be ~20% faster, but this is not worth the extra stutter at lower slot counts)
std::vector<Wasp> wasps{};

std::vector<Wasp>* WaspSlots::getWasps()
{
    return &wasps;
}

/**
* Initializes all wasp slots. 
* Do not call spawnWasps() before initialization.
*/
void WaspSlots::init()
{
    // Since std::vector always maintains locality (moves all elements to a larger 
    // empty spot on the heap when the currently reserved space is filled),  
    // emplace_back() can safely be used at allocation time without risk of breaking locality.
    for (int i = 0; i < SLOT_COUNT; i++)
    {
        wasps.emplace_back(Wasp(i));
    }
}

/**
* Returns an index that can be used as the upper bound for loops.
* After this index there are no more living wasp objects in the list.
* This optimization relies heavily on well managed calls to updateMaxIndex()
* and on killing wasps with higher indices before wasps with smaller indices.
*/
int WaspSlots::getMaxIndex()
{
    return wasp_maxIndex;
}

/**
* Updates maxIndex by finding the largest index containing a living wasp in the wasp vector.
*/
void WaspSlots::updateMaxIndex()
{
    int index = -1;
    for (int i = 0; i < SLOT_COUNT; ++i)
    {
        Wasp* wasp = &wasps[i];
        if (wasp->isAlive) { index = i; }
    }
    wasp_maxIndex = index >= 0 ? index + 1 : 0; // +1 here to make using it for loops easier
}

/**
* Returns wether there are enough wasp slots available to accommodate waspAddAmount.
*/
bool WaspSlots::spaceAvailable(int waspAddAmount)
{
    return aliveCount + waspAddAmount <= SLOT_COUNT;
}

/**
* Returns the total number of currently living wasps.
*/
int WaspSlots::getAliveCount()
{
    return aliveCount;
}

/**
* Returns the total number of wasp deaths since initialization.
*/
long WaspSlots::getDeadCount()
{
    return deadCount;
}

/**
* Used to register the death of a wasp and thereby update aliveCount and deadCount.
*/
void WaspSlots::registerDeath()
{
    aliveCount--;
    deadCount++;
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
    for (int i = 0; i < SLOT_COUNT && spawnedAmount < amount; ++i)
    {
        Wasp* wasp = &wasps[i];
        if (wasp->isAlive) { continue; }
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

    updateMaxIndex();
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

    // Note: Even though the default KillStrategy is named 'RANDOM', in reality wasps are
    // always killed based on index starting from the back of the vector. This ensures
    // that wasp_maxIndex is always as small as possible when using the kill function.
    // When using a random SpawnStrategy this index based approach still looks random.
    for (int i = SLOT_COUNT - 1; i >= 0 && killedAmount != amountToKill; --i)
    {
        Wasp* wasp = &wasps[i];
        if (!wasp->isAlive) { continue; }
        
        wasp->kill();
        killedAmount++;
    }

    updateMaxIndex();

    aliveCount -= killedAmount;
    deadCount += killedAmount;
    return killedAmount;
}