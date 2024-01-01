/**
 * @file Wasp.h
 * @brief This file serves as the header for wasp.cpp.
 */

#pragma once
#include <DirectXMath.h>

/**
* @class Wasp
* @brief A single wasp within the simulation.
*/
#include "Updatable.h"

class Wasp : Updatable
{
	public:

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
		* Implementation/Override of 'Updatable's update method. Updates the wasps state in the simulation.
		*/
		void update();

	private:
		DirectX::XMFLOAT3 position;
};