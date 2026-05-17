#pragma once

#include "Updatable.h"
#include "WaspGenes.h"
#include "Food.h"
#include <chrono>
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
		* Implementation/Override of the 'Updatable' class update method. Updates the wasps state in the simulation.
		*/
		void update();

		// ---GENES---
		WaspGenes::UnboundGenes unboundGenes;
		WaspGenes::BalancedGenes balancedGenes;

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

		//GOAL
		glm::vec3* currentGoal;
		Food::FoodEntity* currentGoalFoodEntity;

	// Note: While I would prefer more attributes to be private, the overhead of getter functions
	// is just not worth it at very high wasp counts
	private:
		Queen& queen;
		const int QUEEN_INTERACTION_TIMEOUT = 1000; 
		int queenInteractionCountdown;
		
		std::chrono::steady_clock::time_point lastResourceTick;

		void initialize();

		inline void lookAroundRandomly(double deltaTime);

		inline void turnTowardsGoal();

		/**
		* Gives the given amount of food to the queen and thereby decreases the wasp's own hungerSaturation.
		* Does not check food availability or distance to the queen.
		*/
		void giveFoodToQueen(int amount);

		/**
		* Shared death logic between all different ways that a wasp can die.
		*/
		inline void die();
};
