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

		glm::vec3 getPosition();

		void setPosition(glm::vec3 position);

		glm::vec3 getViewingVector();

		void setViewingVector(glm::vec3 viewingVector);

		bool isAlive();

		void kill();

	private:
		glm::vec3 position;
		glm::vec3 viewingVector;

		float turnSpeed; // speed of rotating around the y axis
		float ascendSpeed; // speed of movement along the y axis

		std::chrono::duration<double>* deltaTime;

		bool _isAlive;

		/**
		* Updates the Wasp's position based on its current state
		*/
		void updatePosition();

		/**
		* Updates the Wasp's viewingVector based on its current state
		*/
		void updateViewingVector();
};