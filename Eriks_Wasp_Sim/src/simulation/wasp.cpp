/**
 * @file Wasp.cpp
 * @brief This file specifies the implementation of wasp.h
 */

#include "Wasp.h"
#include <random>
#include <cmath>
#include "Simulation.h"
#include "SimVisualizer.h"

using namespace DirectX;

/**
* Creates a Wasp object
*/
Wasp::Wasp()
{
	position = XMFLOAT3(0, 0, 0);
	viewingVector = XMFLOAT3(0, 0, 1);

	turnSpeed = 0;
	ascendSpeed = 0;

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
	// ROTATE AROUND Y AXIS
	float theta = turnSpeed * deltaTime->count();

	DirectX::XMFLOAT3 tempVV = viewingVector;
	viewingVector.x = tempVV.x * cos(theta) + tempVV.z * sin(theta);
	viewingVector.z = -tempVV.x * sin(theta) + tempVV.z * cos(theta);

	// FLY UP OR DOWN
	viewingVector.y = ascendSpeed;

	// CHANGE GOAL VIEWING VECTOR
	static const double directionSwitchSeconds = 3;
	static double secondsUntilDirectionSwitch = 0;

	secondsUntilDirectionSwitch -= deltaTime->count();

	if (secondsUntilDirectionSwitch < 0.0)
	{
		secondsUntilDirectionSwitch = directionSwitchSeconds;

		switch (std::rand() % 5)
		{
			case 0:	
				turnSpeed = 0;
				break;
			case 1: 
				turnSpeed = ((float)std::rand() / RAND_MAX) * 2.0f - 1.0f;
				break;
			case 2: 
				ascendSpeed = ((float)std::rand() / RAND_MAX) * 2.0f - 1.0f;
				break;
			case 3: 
				turnSpeed = ((float)std::rand() / RAND_MAX) * 2.0f - 1.0f;
				ascendSpeed = ((float)std::rand() / RAND_MAX) * 2.0f - 1.0f;
				break;
			default: 
				break;
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
