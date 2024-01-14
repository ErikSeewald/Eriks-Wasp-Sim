/**
 * @file Wasp.cpp
 * @brief This file specifies the implementation of wasp.h
 */

#include "Wasp.h"
#include <random>
using DirectX::XMFLOAT3;

/**
* Implementation/Override of 'Updatable's update method. Updates the wasps state in the simulation.
*/
void Wasp::update()
{
	switch (std::rand() % 6)
	{
	case 0: this->position.x += 0.01f;  break;
	case 1: this->position.y += 0.01f;  break;
	case 2: this->position.z += 0.01f;  break;
	case 3: this->position.x -= 0.01f;  break;
	case 4: this->position.y -= 0.01f;  break;
	case 5: this->position.z -= 0.01f;  break;
	}
	
}

/**
* Returns the 3D position vector of this Wasp
*
* @return the position as XMFLOAT3
*/
XMFLOAT3 Wasp::getPosition() 
{
	return this->position;
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
