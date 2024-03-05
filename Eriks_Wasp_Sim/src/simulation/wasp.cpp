#include "Wasp.h"
#include <random>
#include "Simulation.h"

Wasp::Wasp()
{
	MAX_HP = 100;
	hp = MAX_HP;

	position = glm::vec3(0, 0, 0);
	viewingVector = glm::vec3(0, 0, 1);

	turnSpeed = 0;
	ascendSpeed = 0;

	_isAlive = true;

	deltaTime = Simulation::getDeltaTime();
}

/**
* Implementation/Override of the 'Updatable' class update method. Updates the wasps state in the simulation.
*/
void Wasp::update()
{
	deltaTime = Simulation::getDeltaTime();

	updatePosition();
	updateViewingVector();
}


// POSITION
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

glm::vec3 Wasp::getPosition() const
{
	return position;
}

void Wasp::setPosition(glm::vec3 position)
{
	this->position = position;
}


// MOVEMENT
/**
* Updates the Wasp's viewingVector based on its current state
*/
void Wasp::updateViewingVector()
{
	// ROTATE AROUND Y AXIS
	float theta = turnSpeed * deltaTime->count();

	glm::vec3 tempVV = viewingVector;
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

int Wasp::getMaxHP() const
{
	return MAX_HP;
}

bool Wasp::isAlive() const
{
	return _isAlive;
}

void Wasp::kill()
{
	hp = 0;
	_isAlive = false;
}