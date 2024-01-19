/**
 * @file Wasp.cpp
 * @brief This file specifies the implementation of wasp.h
 */

#include "Wasp.h"
#include <random>
#include <cmath>
#include "Simulation.h"

using namespace DirectX;

/**
* Creates a Wasp object
*/
Wasp::Wasp()
{
	position = XMFLOAT3(0, 0, 0);
	viewingVector = XMFLOAT3(0, 0, 1);
	deltaTime = Simulation::getDeltaTime();
}

/**
* Implementation/Override of 'Updatable's update method. Updates the wasps state in the simulation.
*/
void Wasp::update()
{
	deltaTime = Simulation::getDeltaTime();

	updatePosition();
	updateViewingVector();
}

/**
* Updates the Wasp's position based on its current state
*/
void Wasp::updatePosition()
{
	float speedMultiplier = 0.8f * deltaTime->count(); // speed of 5 units per second

	position.x += viewingVector.x * speedMultiplier;
	position.y += viewingVector.y * speedMultiplier;
	position.z += viewingVector.z * speedMultiplier;
}

/**
* Updates the Wasp's viewingVector based on its current state
*/
void Wasp::updateViewingVector()
{
	// Wasp has a chance to switch is directione every 2 seconds
	static const double directionSwitchSeconds = 2;
	static double secondsUntilDirectionSwitch = directionSwitchSeconds;

	secondsUntilDirectionSwitch -= deltaTime->count();

	if (secondsUntilDirectionSwitch < 0.0)
	{
		secondsUntilDirectionSwitch = directionSwitchSeconds;

		if (std::rand() % 3 == 0) // 30 % chance to switch direction
		{
			viewingVector.x += ((float)(std::rand() % 100) - 50) / 10;
			viewingVector.y += ((float)(std::rand() % 100) - 50) / 10;
			viewingVector.z += ((float)(std::rand() % 100) - 50) / 10;

			XMVECTOR normalized = XMVector3Normalize(XMLoadFloat3(&viewingVector));
			XMStoreFloat3(&viewingVector, normalized);
		}
	}
}

/**
* Returns the 3D position vector of this Wasp
*
* @return the position as XMFLOAT3
*/
XMFLOAT3 Wasp::getPosition() 
{
	return position;
}

/**
* Sets the 3D position vector of this Wasp to the given XMFLOAT3
*
* @param position - the XMFLOAT3 to set the Wasp's position to
*/
void Wasp::setPosition(XMFLOAT3 position)
{
	this->position = position;
}

/**
* Returns the 3D viewing vector of this Wasp
*
* @return the viewing vector as XMFLOAT3
*/
DirectX::XMFLOAT3 Wasp::getViewingVector()
{
	return viewingVector;
}

/**
* Sets the 3D viewing vector of this Wasp to the given XMFLOAT3
*
* @param position - the XMFLOAT3 to set the Wasp's viewing vector to
*/
void Wasp::setViewingVector(DirectX::XMFLOAT3 viewingVector)
{
	this->viewingVector = viewingVector;
}
