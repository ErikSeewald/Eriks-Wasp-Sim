#pragma once

/**
* @namespace EntitySlots
* @brief A namespace of methods handling linked slot lists of entities in the simulation.
*/
namespace EntitySlots
{
	/**
	* @struct EntitySlot
	* @brief A struct to use for linked lists of slots containing entities. Dereferencing the void pointer is the users responsibility.
	*/
	struct EntitySlot
	{
		void* entity;
		EntitySlot* prev;
		EntitySlot* next;
	};

	/**
	* Creates and allocates space for a new EntitySlot in the linked list defined by slotsStart and SlotsEnd
	*/
	void allocateSlot(void* entity, EntitySlot** slotsStart, EntitySlot** slotsEnd);

	/**
	* Removes the given slot from the linked list defined by slotsStart and SlotsEnd and schedules
	* its deletion from memory.
	*/
	void removeSlot(EntitySlot* slot, EntitySlot** slotsStart, EntitySlot** slotsEnd);

	/**
	* Deletes and cleans up all entity slots that have been marked as deletable.
	*/
	void cleanupMemory();
}