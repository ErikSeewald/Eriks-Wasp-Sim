#include "Wasp.h"
#include "Simulation.h"

using Simulation::SpawnStrategy;
using Simulation::KillStrategy;

/**
* @namespace WaspSlots
* @brief A namespace to handle the global WaspSlot linked list.
*/
namespace WaspSlots
{
	/**
	* @struct WaspSlot
	* @brief Linked list struct with a pointer to its own wasp and pointers to the previous and next WaspSlot.
	*/
	struct WaspSlot
	{
		Wasp* wasp;
		WaspSlot* next;
		WaspSlot* prev;
	};

	/**
	* Allocates space in the wasp slot linked list and creates a slot for the given wasp.
	*
	* @param wasp the wasp to allocate a slot for
	*/
	void allocateWaspSlot(Wasp* wasp);

	/**
	* Removes the given wasp slot from the linked list and marks both the slot and the wasp it
	* points to for deletion.
	*
	* @param waspSlot the slot to remove
	*/
	void removeWaspSlot(WaspSlot* waspSlot);

	WaspSlot* getWaspSlots();

	bool spawnWasps(glm::vec3 position, int amount, SpawnStrategy strategy, float SpawnRadius);

	int killWasps(int amountToKill, KillStrategy strategy);

	bool spaceAvailable(int wasAddAmount);

	/**
	* Deletes and cleans up all wasp slots that have been marked as deletable
	*/
	void cleanupMemory();

	int getAliveCount();

	long getDeadCount();
}