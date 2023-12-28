/**
 * @file Wasp.cpp
 * @brief This file specifies the implementation of wasp.h
 */

#include "Wasp.h"
using DirectX::XMFLOAT3;

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
