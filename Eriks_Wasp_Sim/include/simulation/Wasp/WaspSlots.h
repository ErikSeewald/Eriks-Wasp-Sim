#include "Wasp.h"
#include "Simulation.h"
#include "EntitySlots.h"

using EntitySlots::EntitySlot;
using Simulation::SpawnStrategy;
using Simulation::KillStrategy;

/**
* @namespace WaspSlots
* @brief A namespace to handle the global WaspSlot linked list.
*/
namespace WaspSlots
{
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
	void removeWaspSlot(EntitySlot* waspSlot);

	EntitySlot* getWaspSlots();

	bool spawnWasps(glm::vec3 position, int amount, SpawnStrategy strategy, float SpawnRadius);

	int killWasps(int amountToKill, KillStrategy strategy);

	bool spaceAvailable(int waspAddAmount);

	int getAliveCount();

	long getDeadCount();
}