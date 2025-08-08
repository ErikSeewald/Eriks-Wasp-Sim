#pragma once

#include "Updatable.h"
#include "Food.h"
#include <chrono>
#include <glm/glm.hpp>

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

		// Position in the wasp vector
		const int w_Index;

		/**
		* Constructs the wasp object and assigns it the given w_Index (which indicates its position in the wasp vector).
		* By default the wasp is constructed with isAlive = false.
		*/
		Wasp(const int w_Index);

		// Default constructor, only exists for the compiler
		Wasp();

		/**
		* Respawns the wasp and resets its attributes. Both living and dead wasps can be respawned.
		*/
		void respawn();

		/**
		* Implementation/Override of the 'Updatable' class update method. Updates the wasps state in the simulation.
		*/
		void update();


		// ---MOVEMENT---
		glm::vec3 position;
		glm::vec3 viewingVector;

		float flyingSpeed;
		float turnSpeed; // speed of rotating around the y axis
		float ascendSpeed; // speed of movement along the y axis


		// ---HEALTH---
		bool isAlive;
		int MAX_HP;
		int hp;

		/**
		* Kills the wasp. Until this wasp is respawned, isAlive will be false.
		* Dead wasp objects are kept in memory but are no longer updated or rendered.
		*/
		void kill();


		// ---HUNGER---
		int MAX_HUNGER_SATURATION;
		int hungerSaturation;

		void onFoodReached();

		//GOAL
		glm::vec3* currentGoal;
		Food::FoodEntity* currentGoalFoodEntity;

	// Note: While I would prefer more attributes to be private, the overhead of getter functions
	// is just not worth it at very high wasp counts
	private:
		std::chrono::steady_clock::time_point lastResourceTick;

		void initialize();

		inline void lookAroundRandomly(double deltaTime);

		inline void turnTowardsGoal();
};
