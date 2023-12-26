#include "wasp.h"

XMFLOAT3 Wasp::getPosition() 
{
	return this->position;
}

void Wasp::setPosition(XMFLOAT3 position)
{
	this->position = position;
}
