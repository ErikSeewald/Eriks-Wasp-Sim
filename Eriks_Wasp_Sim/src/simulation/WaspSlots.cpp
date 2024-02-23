#include "WaspSlots.h"
#include <list>

using WaspSlots::WaspSlot;

//WASPS
WaspSlot* waspSlotsStart;
WaspSlot* waspSlotsEnd;;

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
}

WaspSlot* WaspSlots::getWaspSlots()
{
    return waspSlotsStart;
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