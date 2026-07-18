#pragma once

#include "Updatable.h"
#include "WaspGenes.h"
#include "Food.h"
#include "Contracts.h"
#include <chrono>
#include <array>
#include <glm/glm.hpp>

class Queen; // Forward declaration so the queen can be referenced here

/**
* @class Wasp
* @brief A single wasp within the simulation.
*/
class Wasp : Updatable
{
	public:
		// CONSTANTS
		static constexpr int VIEW_RANGE = 10; // How far the wasp can see other entities
		static constexpr std::chrono::milliseconds RESOURCE_TICK_INTERVAL{2000}; // How often do resource attributes like hp and hunger get ticked
		static constexpr int MAX_NUM_CONTRACTS = 4; // A limit here should make for more interestic cost-benefit decisions
		const int w_Index; // Position in the wasp vector

		/**
		* Constructs the wasp object and assigns it the given w_Index (which indicates its position in the wasp vector).
		* Also assigns the reference to the queen. Every wasp knows its queen.
		* By default the wasp is constructed with isAlive = false.
		*/
		Wasp(const int w_Index, Queen& queen);

		/**
		* Respawns the wasp and resets its attributes. Both living and dead wasps can be respawned.
		*/
		void respawn();

		/**
		*  Sets the 'privileged' state of the wasp, dictating whether it is allowed to perform certain expensive computations.
		*/ 
		void setPrivileged(bool privileged);

		/**
		* Implementation/Override of the 'Updatable' class update method. Updates the wasps state in the simulation.
		*/
		void update();

		// ---GENES---
		WaspGenes::UnboundGenes unboundGenes;
		WaspGenes::BalancedGenes balancedGenes;

		// ---CONTRACTS---
		// A wasp can only have a maximum number of contracts and ONLY ONE per ContractType.
		std::array<Contracts::Contract*, MAX_NUM_CONTRACTS> contracts;

		/**
		* Allows the given wasp to propose a contract of the given type to this wasp.
		* Performs the terms negotiation and creates the contract if the proposal was accepted.
		* Returns a pointer to the newly created contract or nullptr if the proposal was rejected.
		*/
		Contracts::Contract* receiveNewContractProposal(Wasp* proposer, Contracts::ContractType type);

		/**
		* Allows the given wasp to propose joining the given contract to this wasp.
		* Returns true if that proposal was accepted. In that case,
		* the function already adds this wasp to the list of partners.
		*/
		bool receiveContractJoinProposal(Wasp* proposer, Contracts::Contract* contract);

		// ---MOVEMENT---
		glm::vec3 position;
		glm::vec3 viewingVector;

		float turnSpeed; // Speed of rotating around the y axis
		float ascendSpeed; // Speed of movement along the y axis

		// ---HEALTH---
		bool isAlive;
		int hp;

		/**
		* Kills the wasp. Until this wasp is respawned, isAlive will be false.
		* Dead wasp objects are kept in memory but are no longer updated or rendered.
		*/
		void kill();

		// ---HUNGER---
		int hungerSaturation;

		void onFoodReached();

		/**
		* Adds the given amount of points to the wasp's hunger saturation
		* while limiting itself to its maxHungerSaturation.
		* Returns the amount of points that could not be consumed due to
		* going over the maximum.
		*/
		int addHungerSaturationBounded(int saturationPoints);

		//GOAL
		glm::vec3* currentGoal;
		Food::FoodEntity* currentGoalFoodEntity;

		// DEBUG ONLY
		float _debugWorkerScore = 0.0; // Only the score queried from the queen is actually valid, this is only for debug rendering.

	// Note: While I would prefer more attributes to be private, the overhead of getter functions
	// is just not worth it at very high wasp counts
	private:
		// QUEEN INTERACTION
		Queen& queen;
		const int QUEEN_INTERACTION_TIMEOUT = 1000; 
		int queenInteractionCountdown;
		
		// NORMAL INTERACTION AND PRIVILEGE 
		std::chrono::steady_clock::time_point lastResourceTick;
		bool isPrivileged; // Whether the wasp is currently allowed to perform certain expensive computations


		void initialize();

		inline void lookAroundRandomly(double deltaTime);

		inline void turnTowardsGoal();

		/**
		* Returns an index of a contract slot that is available to be overwritten (is nullptr or expired).
		* Returns -1 if no such index could be found. 
		*/
		int getAvailableContractIndex();

		/**
		* Wasps are only allowed a maximum of one active contract per type. Therefore,
		* this function either returns the index of that contract or -1 if the wasp
		* does not yet have a (still active) contract of the given type.
		*/
		int getContractIndexByType(Contracts::ContractType type);

		/**
		 * By random chance the wasp can try to propose a contract to another wasp.
		 */
		void tryProposeContract(double deltaTime);

		/**
		* Considers interest in the contract based on type-specific conditions and returns
		* whether the wasp should accept (true) or reject (false) the proposal.
		* The given interestFactor can be used to scale the base interest in the contract.
		* Note: Checking whether an available slot for the contract is even free is the
		* responsibility of the caller.
		*/
		bool considerAcceptingContract(Wasp* proposer, Contracts::ContractType type, float interestFactor);

		/**
		* Gives the given amount of food to the queen and thereby decreases the wasp's own hungerSaturation.
		* Limits the amount to the actual available amount of food.
		* Does not check food availability or distance to the queen.
		*/
		void giveFoodToQueen(int amount);

		/**
		* Shared death logic between all different ways that a wasp can die.
		*/
		inline void die();
};
