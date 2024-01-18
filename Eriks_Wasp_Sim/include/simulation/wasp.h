/**
 * @file Wasp.h
 * @brief This file serves as the header for wasp.cpp.
 */

#pragma once
#include <DirectXMath.h>
#include <chrono>

/**
* @class Wasp
* @brief A single wasp within the simulation.
*/
#include "Updatable.h"

class Wasp : Updatable
{
	public:

		/**
		* Creates a Wasp object
		*/
		Wasp();

		/**
		* Returns the 3D position vector of this Wasp
		* 
		* @return the position as XMFLOAT3
		*/
		DirectX::XMFLOAT3 getPosition();

		/**
		* Sets the 3D position vector of this Wasp to the given XMFLOAT3
		*
		* @param position - the XMFLOAT3 to set the Wasp's position to
		*/
		void setPosition(DirectX::XMFLOAT3 position);

		/**
		* Returns the 3D viewing vector of this Wasp
		*
		* @return the viewing vector as XMFLOAT3
		*/
		DirectX::XMFLOAT3 getViewingVector();

		/**
		* Sets the 3D viewing vector of this Wasp to the given XMFLOAT3
		*
		* @param position - the XMFLOAT3 to set the Wasp's viewing vector to
		*/
		void setViewingVector(DirectX::XMFLOAT3 viewingVector);

		/**
		* Implementation/Override of 'Updatable's update method. Updates the wasps state in the simulation.
		*/
		void update();

	private:
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 viewingVector;
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