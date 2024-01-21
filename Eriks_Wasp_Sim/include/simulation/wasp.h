#pragma once
#include <DirectXMath.h>
#include <chrono>
#include "Updatable.h"

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

		DirectX::XMFLOAT3 getPosition();

		void setPosition(DirectX::XMFLOAT3 position);

		DirectX::XMFLOAT3 getViewingVector();

		void setViewingVector(DirectX::XMFLOAT3 viewingVector);

	private:
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 viewingVector;

		float turnSpeed; // speed of rotating around the y axis
		float ascendSpeed; // speed of movement along the y axis

		std::chrono::duration<double>* deltaTime;

		/**
		* Updates the Wasp's position based on its current state
		*/
		void updatePosition();

		/**
		* Updates the Wasp's viewingVector based on its current state
		*/
		void updateViewingVector();
};