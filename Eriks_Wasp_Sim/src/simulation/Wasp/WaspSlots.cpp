#include "WaspSlots.h"
#include "MemoryManager.h"
#include <list>

using WaspSlots::WaspSlot;
using Simulation::SpawnStrategy;
using Simulation::KillStrategy;

//WASPS
WaspSlot* waspSlotsStart;
WaspSlot* waspSlotsEnd;
int aliveCount = 0;
long deadCount = 0;
const int MAX_WASP_COUNT = 100000;

// List to remember which wasp slots to delete in cleanupMemory(). 
// Deleting them immediately would cause concurrency issues.
std::list<WaspSlot*> waspSlotsToDelete{};

/**
* Allocates space in the wasp slot linked list and creates a slot for the given wasp.
* 
* @param wasp the wasp to allocate a slot for
*/
void WaspSlots::allocateWaspSlot(Wasp* wasp)
{
    WaspSlot* waspSlot = new WaspSlot();
    waspSlot->wasp = wasp;
    waspSlot->prev = waspSlotsEnd;
    waspSlot->next = nullptr;

    // If allocating first element
    if (waspSlotsStart == nullptr)
    {
        waspSlotsStart = waspSlot;
        waspSlotsEnd = waspSlot;
    }

    // Otherwise allocate at waspSlotsEnd
    else
    {
        waspSlotsEnd->next = waspSlot;
        waspSlotsEnd = waspSlot;
    }

    aliveCount++;
}

/**
* Removes the given wasp slot from the linked list and marks both the slot and the wasp it
* points to for deletion.
* 
* @param waspSlot the slot to remove
*/
void WaspSlots::removeWaspSlot(WaspSlot* waspSlot)
{   
    // REMOVING START SLOT
    if (waspSlot == waspSlotsStart)
    {
        if (waspSlot->next == nullptr)
        {
            waspSlotsStart = nullptr;
            waspSlotsEnd = nullptr;
        }
        
        else
        {
            waspSlotsStart = waspSlot->next;
            waspSlotsStart->prev = nullptr;
        }
    }

    else
    {
        WaspSlot* prev = waspSlot->prev;

        // REMOVING END SLOT
        if (waspSlot == waspSlotsEnd)
        {
            prev->next = nullptr;
            waspSlotsEnd = prev;
        }

        // REMOVING OTHER SLOTS
        else
        {
            WaspSlot* next = waspSlot->next;
            prev->next = next;
            next->prev = prev;
        }
    }

    waspSlotsToDelete.push_back(waspSlot);
    aliveCount--;
    deadCount++;
}

WaspSlot* WaspSlots::getWaspSlots()
{
    return waspSlotsStart;
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
    WaspSlot* waspSlot = WaspSlots::getWaspSlots();
    while (waspSlot != nullptr && killedAmount != amountToKill)
    {
        waspSlot->wasp->kill();
        killedAmount++;
        waspSlot = waspSlot->next;
    }

    MemoryManager::scheduleCleanup();
    return killedAmount;
}

bool WaspSlots::spaceAvailable(int waspAddAmount)
{
    return aliveCount + waspAddAmount <= MAX_WASP_COUNT;
}


/**
* Deletes and cleans up all wasp slots that have been marked as deletable
*/
void WaspSlots::cleanupMemory()
{
    for (WaspSlot* waspSlot : waspSlotsToDelete)
    {
        delete waspSlot->wasp;
        delete waspSlot;
    }
    waspSlotsToDelete.clear();
}

int WaspSlots::getAliveCount()
{
    return aliveCount;
}

long WaspSlots::getDeadCount()
{
    return deadCount;
}