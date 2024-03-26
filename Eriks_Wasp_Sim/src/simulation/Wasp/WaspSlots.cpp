#include "WaspSlots.h"
#include "MemoryManager.h"

using EntitySlots::EntitySlot;
using Simulation::SpawnStrategy;
using Simulation::KillStrategy;

EntitySlot* waspSlotsStart;
EntitySlot* waspSlotsEnd;
int aliveCount = 0;
long deadCount = 0;
const int MAX_WASP_COUNT = 100000;


/**
* Allocates space in the wasp slot linked list and creates a slot for the given wasp.
* 
* @param wasp the wasp to allocate a slot for
*/
void WaspSlots::allocateWaspSlot(Wasp* wasp)
{
    EntitySlots::allocateSlot(wasp, &waspSlotsStart, &waspSlotsEnd);
    aliveCount++;
}

/**
* Removes the given wasp slot from the linked list and marks both the slot and the wasp it
* points to for deletion.
* 
* @param waspSlot the slot to remove
*/
void WaspSlots::removeWaspSlot(EntitySlot* waspSlot)
{   
    EntitySlots::removeSlot(waspSlot, &waspSlotsStart, &waspSlotsEnd);

    aliveCount--;
    deadCount++;
}

EntitySlot* WaspSlots::getWaspSlots()
{
    return waspSlotsStart;
}

bool WaspSlots::spaceAvailable(int waspAddAmount)
{
    return aliveCount + waspAddAmount <= MAX_WASP_COUNT;
}

int WaspSlots::getAliveCount()
{
    return aliveCount;
}

long WaspSlots::getDeadCount()
{
    return deadCount;
}

bool WaspSlots::spawnWasps(glm::vec3 position, int amount, SpawnStrategy strategy, float spawnRadius)
{
    if (!spaceAvailable(amount))
    {
        return false;
    }

    if (strategy == SpawnStrategy::RANDOM)
    {
        for (int i = 0; i < amount; i++)
        {
            Wasp* wasp = new Wasp();

            double x = ((((float)std::rand() / RAND_MAX) * 2.0) - 1.0) * spawnRadius;
            double y = ((((float)std::rand() / RAND_MAX) * 2.0) - 1.0) * spawnRadius;
            double z = ((((float)std::rand() / RAND_MAX) * 2.0) - 1.0) * spawnRadius;
            wasp->setPosition(glm::vec3(position.x + x, position.y + y, position.z + z));

            WaspSlots::allocateWaspSlot(wasp);
        }
    }

    else
    {
        for (int i = 0; i < amount; i++)
        {
            Wasp* wasp = new Wasp();
            wasp->setPosition(position);
            WaspSlots::allocateWaspSlot(wasp);
        }
    }

    return amount >= 1;
}


int WaspSlots::killWasps(int amountToKill, KillStrategy strategy)
{
    if (strategy == KillStrategy::ALL)
    {
        amountToKill = -1;
    }

    int killedAmount = 0;
    EntitySlot* waspSlot = WaspSlots::getWaspSlots();
    while (waspSlot != nullptr && killedAmount != amountToKill)
    {
        Wasp* wasp = (Wasp*)waspSlot->entity;
        wasp->kill();
        killedAmount++;
        waspSlot = waspSlot->next;
    }

    MemoryManager::scheduleCleanup();
    return killedAmount;
}