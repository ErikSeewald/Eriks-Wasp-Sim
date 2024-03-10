#pragma once

#include <chrono>
#include "Updatable.h"
#include <glm/glm.hpp>

/**
* @class Wasp
* @brief A single wasp within the simulation.
*/
class Wasp : Updatable
{
	public:

		Wasp();

		/**
		* Implementation/Override of the 'Updatable' class update method. Updates the wasps state in the simulation.
		*/
		void update();

		// POSITION
		glm::vec3 getPosition() const;

		void setPosition(glm::vec3 position);

		// MOVEMENT
		glm::vec3 getViewingVector() const;

		float getTurnSpeed() const;

		float getAscendSpeed() const;

		void setViewingVector(glm::vec3 viewingVector);

		void setCurrentGoal(glm::vec3* goal);

		glm::vec3* getCurrentGoal();

		// HEALTH
		int getHP() const;

		int getMaxHP() const;

		bool isAlive() const;

		void kill();

	private:
		std::chrono::duration<double>* deltaTime;

		void updateSpeeds();

		// POSITION
		glm::vec3 position;

		/**
		* Updates the Wasp's position based on its current state
		*/
		void updatePosition();

		// MOVEMENT
		glm::vec3 viewingVector;
		float flyingSpeed;

		/**
		* Updates the Wasp's viewingVector based on its current state
		*/
		void updateViewingVector();

		void lookAroundRandomly();

		float turnSpeed; // speed of rotating around the y axis
		float ascendSpeed; // speed of movement along the y axis

		glm::vec3* currentGoal;

		void turnTowardsGoal();

		// HEALTH
		int MAX_HP;
		int hp;
		bool _isAlive;
};