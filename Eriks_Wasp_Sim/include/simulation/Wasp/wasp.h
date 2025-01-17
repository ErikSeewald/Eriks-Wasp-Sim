#pragma once

#include <chrono>
#include "Updatable.h"
#include "food.h"
#include <glm/glm.hpp>

using namespace std::chrono;

/**
* @class Wasp
* @brief A single wasp within the simulation.
*/
class Wasp : Updatable
{
	public:

		Wasp();

		/**
		* Respawns the wasp and resets its attributes. Both living and dead wasps can be respawned.
		*/
		void respawn();

		/**
		* Implementation/Override of the 'Updatable' class update method. Updates the wasps state in the simulation.
		*/
		void update();

		/**
		* Subroutine of the udpate() function responsible for updating the current goal.
		*/
		void updateGoal();

		// MOVEMENT
		float getTurnSpeed() const;

		float getAscendSpeed() const;

		void setGoal(glm::vec3* goal);

		glm::vec3* getCurrentGoal();

		// HEALTH
		int getHP() const;

		void setHP(int newHP);

		int getMaxHP() const;

		/**
		* Returns wether the wasp is alive. 
		* Dead wasp objects are kept in memory but are no longer updated or rendered.
		*/
		bool isAlive() const;

		/**
		* Kills the wasp. Until this wasp is respawned, isAlive() will return false.
		*/
		void kill();

		// HUNGER
		int getHungerSaturation() const;

		void setHungerSaturation(int newSaturation);

		int getMaxHungerSaturation() const;

		void onFoodReached();

		// I would prefer these to be private, but the overhead of a getter function
		// just is not worth it once you reach like 100000 wasps
		glm::vec3 position;
		glm::vec3 viewingVector;

	private:
		std::chrono::duration<double>* deltaTime;

		void initialize();

		void updateSpeeds();

		// POSITION
		/**
		* Updates the Wasp's position based on its current state
		*/
		void updatePosition();

		// MOVEMENT
		float flyingSpeed;
		float turnSpeed; // speed of rotating around the y axis
		float ascendSpeed; // speed of movement along the y axis

		//GOAL
		glm::vec3* currentGoal;
		Food::FoodEntity* currentGoalFoodEntity;

		/**
		* Updates the Wasp's viewingVector based on its current state
		*/
		void updateViewingVector();

		void lookAroundRandomly();

		void turnTowardsGoal();

		// HEALTH
		int MAX_HP;
		int hp;
		bool _isAlive;

		void updateHP();

		// HUNGER
		int MAX_HUNGER_SATURATION;
		int hungerSaturation;

		const double secondsBetweenHungerDecreases = 5.0;
		steady_clock::time_point lastHungerDecrease;

		void updateHunger();
};