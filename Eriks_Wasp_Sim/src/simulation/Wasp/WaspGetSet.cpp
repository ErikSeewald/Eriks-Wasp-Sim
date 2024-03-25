#include "wasp.h"

// NOTE: This file only contains getters and setters for the Wasp object to avoid cluttering wasp.cpp

// POSITION
glm::vec3 Wasp::getPosition() const
{
	return position;
}

void Wasp::setPosition(glm::vec3 position)
{
	this->position = position;
}


// MOVEMENT
void Wasp::setCurrentGoal(glm::vec3* goal)
{
	currentGoal = goal;
}

glm::vec3* Wasp::getCurrentGoal()
{
	return currentGoal;
}

glm::vec3 Wasp::getViewingVector() const
{
	return viewingVector;
}

void Wasp::setViewingVector(glm::vec3 viewingVector)
{
	this->viewingVector = viewingVector;
}

float Wasp::getTurnSpeed() const
{
	return turnSpeed;
}

float Wasp::getAscendSpeed() const
{
	return ascendSpeed;
}


//HEALTH
int Wasp::getHP() const
{
	return hp;
}

void Wasp::setHP(int newHP)
{
	if (!_isAlive || newHP < 0 || newHP > MAX_HP)
	{
		return;
	}

	hp = newHP;
}

int Wasp::getMaxHP() const
{
	return MAX_HP;
}

// HUNGER
int Wasp::getHungerSaturation() const
{
	return hungerSaturation;
}

void Wasp::setHungerSaturation(int newSaturation)
{
	if (newSaturation < 0 || newSaturation > MAX_HUNGER_SATURATION)
	{
		return;
	}

	hungerSaturation = newSaturation;
}

int Wasp::getMaxHungerSaturation() const
{
	return MAX_HUNGER_SATURATION;
}