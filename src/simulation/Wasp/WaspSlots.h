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
	std::vector<Wasp>* getWasps();

	/**
	* Initializes all wasp slots.
	* Do not call spawnWasps() before initialization.
	*/
	void init();

	/**
	* Returns an index that can be used as the upper bound for loops.
	* After this index there are no more living wasp objects in the list.
	* This optimization relies heavily on well managed calls to updateMaxIndex()
	* and on killing wasps with higher indices before wasps with smaller indices.
	*/
	int getMaxIndex();

	/**
	* Updates maxIndex by finding the largest index containing a living wasp in the wasp vector.
	*/
	void updateMaxIndex();

	/**
	* Returns wether there are enough wasp slots available to accommodate waspAddAmount.
	*/
	bool spaceAvailable(int waspAddAmount);

	/**
	* Returns the total number of currently living wasps.
	*/
	int getAliveCount();

	/**
	* Returns the total number of wasp deaths since initialization.
	*/
	long getDeadCount();

	/**
	* Used to register the death of a wasp and thereby update aliveCount and deadCount.
	*/
	void registerDeath();

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
