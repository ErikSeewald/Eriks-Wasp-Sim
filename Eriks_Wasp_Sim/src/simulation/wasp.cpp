#include "Wasp.h"
#include "Simulation.h"
#include "RNG.h"
#include "VectorMath.h"

Wasp::Wasp()
{
	deltaTime = Simulation::getDeltaTime();

	// POSITION
	position = glm::vec3(0, 0, 0);

	// MOVEMENT
	currentGoal = nullptr;

	viewingVector = glm::vec3(0, 0, 1);
	flyingSpeed = 0.8;
	turnSpeed = 0;
	ascendSpeed = 0;

	// HEALTH
	MAX_HP = 100;
	hp = MAX_HP;
	_isAlive = true;

	// HUNGER
	MAX_HUNGER_SATURATION = 100;
	hungerSaturation = MAX_HUNGER_SATURATION;

}

/**
* Implementation/Override of the 'Updatable' class update method. Updates the wasps state in the simulation.
*/
void Wasp::update()
{
	if (!_isAlive)
	{
		return;
	}

	deltaTime = Simulation::getDeltaTime();

	updateSpeeds();
	updatePosition();
	updateViewingVector();

	if (hp <= 0)
	{
		_isAlive = false;
	}
}

void Wasp::updateSpeeds()
{
	if (currentGoal != nullptr)
	{
		turnTowardsGoal();
	}

	else
	{
		lookAroundRandomly();
	}
}

// POSITION
/**
* Updates the Wasp's position based on its current state
*/
void Wasp::updatePosition()
{
	float speedMultiplier = flyingSpeed * deltaTime->count();

	position.x += viewingVector.x * speedMultiplier;
	position.y += viewingVector.y * speedMultiplier;
	position.z += viewingVector.z * speedMultiplier;

	//Reached goal
	if (currentGoal != nullptr && glm::length(*currentGoal - position) < 0.5)
	{
		currentGoal = nullptr;
	}
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
}

void Wasp::lookAroundRandomly()
{
	static const double directionSwitchSeconds = 3;
	static double secondsUntilDirectionSwitch = 0;

	secondsUntilDirectionSwitch -= deltaTime->count();

	if (secondsUntilDirectionSwitch < 0.0)
	{
		secondsUntilDirectionSwitch = directionSwitchSeconds;

		switch (RNG::randMod(5))
		{
			case 0:
				turnSpeed = 0;
				break;
			case 1:
				turnSpeed = RNG::randBetween(-1, 1);
				break;
			case 2:
				ascendSpeed = RNG::randBetween(-1, 1);
				break;
			case 3:
				turnSpeed = RNG::randBetween(-1, 1);
				ascendSpeed = RNG::randBetween(-1, 1);
				break;
			default:
				break;
		}
	}
}

void Wasp::turnTowardsGoal()
{
	float heightDifference = currentGoal->y - position.y;

	if (heightDifference > -0.3 && heightDifference < 0.3)
	{
		ascendSpeed = 0;
	}

	else
	{
		ascendSpeed = heightDifference > 0 ? 1 : -1;
	}

	float angle = VectorMath::angleXZ(viewingVector, *currentGoal - position);

	float speed = angle / 2;
	turnSpeed = angle > 0 ? -speed : speed;
	
}

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

bool Wasp::isAlive() const
{
	return _isAlive;
}

void Wasp::kill()
{
	hp = 0;
	_isAlive = false;
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