#include "Wasp.h"
#include "Simulation.h"

using Strategies::SpawnStrategy;
using Strategies::KillStrategy;

/**
* @namespace WaspSlots
* @brief A namespace to handle the global set of wasp slots.
*/
namespace WaspSlots
{ 
	// Total number of wasp slots
	const static int SLOT_COUNT = 100000;

	std::vector<Wasp>* getWasps();

	/**
	* Returns wether there are enough wasp slots available to accommodate waspAddAmount
	*/
	bool spaceAvailable(int waspAddAmount);

	int getAliveCount();

	long getDeadCount();

	/**
	* Spawns the given amount of wasps at the given position using the given SpawnStrategy and spawnRadius.
	* Returns true if the wasps have been spawned and false if not enough slots were available.
	* Since in reality all slots are already filled with dead wasp objects from the start, when wasps are spawned they
	* are actually respawned by resetting that wasp objects attributes and reviving it.
	*/
	bool spawnWasps(glm::vec3 position, int amount, SpawnStrategy strategy, float SpawnRadius);

	/**
	* Kills the given amount of wasps using the given KillStrategy.
	* Returns the amount of wasps that have been killed.
	* This amount can differ from the desired amount if there weren't enough living wasps to kill.
	* Since in reality all slots are always filled with wasp objects, when wasps are killed they
	* still remain in memory and are simply no longer updated until they are respawned.
	*/
	int killWasps(int amountToKill, KillStrategy strategy);
}