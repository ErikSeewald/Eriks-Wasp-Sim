#include "EntitySlots.h"
#include <list>

using EntitySlots::EntitySlot;

// List to remember which slots to delete in cleanupMemory(). 
// Deleting them immediately would cause concurrency issues.
std::list<EntitySlot*> slotsToDelete{};

/**
* Creates and allocates space for a new EntitySlot in the linked list defined by slotsStart and SlotsEnd
*/
void EntitySlots::allocateSlot(void* entity, EntitySlot** slotsStart, EntitySlot** slotsEnd)
{
    EntitySlot* slot = new EntitySlot();
    slot->entity = entity;
    slot->prev = *slotsEnd;
    slot->next = nullptr;

    // If allocating first element
    if (*slotsStart == nullptr)
    {
        *slotsStart = slot;
        *slotsEnd = slot;
    }

    // Otherwise allocate at waspSlotsEnd
    else
    {
        (*slotsEnd)->next = slot;
        *slotsEnd = slot;
    }
}

/**
* Removes the given slot from the linked list defined by slotsStart and SlotsEnd and schedules
* its deletion from memory.
*/
void EntitySlots::removeSlot(EntitySlot* slot, EntitySlot** slotsStart, EntitySlot** slotsEnd)
{
    // REMOVING START SLOT
    if (slot == *slotsStart)
    {
        if (slot->next == nullptr)
        {
            *slotsStart = nullptr;
            *slotsEnd = nullptr;
        }

        else
        {
            *slotsStart = slot->next;
            (*slotsStart)->prev = nullptr;
        }
    }

    else
    {
        EntitySlot* prev = slot->prev;

        // REMOVING END SLOT
        if (slot == *slotsEnd)
        {
            prev->next = nullptr;
            *slotsEnd = prev;
        }

        // REMOVING OTHER SLOTS
        else
        {
            EntitySlot* next = slot->next;
            prev->next = next;
            next->prev = prev;
        }
    }

    slotsToDelete.push_back(slot);
}

/**
* Deletes and cleans up all entity slots that have been marked as deletable.
*/
void EntitySlots::cleanupMemory()
{
    for (EntitySlot* slot : slotsToDelete)
    {
        delete slot->entity;
        delete slot;
    }
    slotsToDelete.clear();
}