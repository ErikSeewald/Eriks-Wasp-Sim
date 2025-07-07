#include "Wasp.h"
#include "Simulation.h"
#include "RNG.h"
#include "VectorMath.h"
#include "WaspSlots.h"

/**
* Constructs the wasp object and assigns it the given w_Index (which indicates its position in the wasp vector).
* By default the wasp is constructed with isAlive = false.
*/
Wasp::Wasp(const int w_Index) : w_Index(w_Index)
{
	initialize();
	isAlive = false;
}

// Default constructor, only exists for the compiler
Wasp::Wasp() : w_Index(-1)
{
	initialize();
	isAlive = false;
}

void Wasp::initialize()
{
	deltaTime = Simulation::getDeltaTime();

	// POSITION
	position = glm::vec3(0, 0, 0);

	//GOAL
	currentGoal = nullptr;
	currentGoalFoodEntity = nullptr;

	// MOVEMENT
	viewingVector = glm::vec3(0, 0, 1);
	flyingSpeed = 0.8;
	turnSpeed = 0;
	ascendSpeed = 0;

	// HEALTH
	MAX_HP = 100;
	hp = MAX_HP;

	// HUNGER
	MAX_HUNGER_SATURATION = 100;
	hungerSaturation = MAX_HUNGER_SATURATION;
	lastHungerDecrease = steady_clock::now();
}

/**
* Respawns the wasp and resets its attributes. Both living and dead wasps can be respawned.
*/
void Wasp::respawn()
{
	initialize();
	isAlive = true;
}

/**
* Implementation/Override of the 'Updatable' class update method. Updates the wasps state in the simulation.
*/
void Wasp::update()
{
	if (!isAlive)
	{
		return;
	}

	if (hp <= 0)
	{
		isAlive = false;
		WaspSlots::registerDeath();
		return;
	}

	deltaTime = Simulation::getDeltaTime();

	updateSpeeds();
	updatePosition();
	updateViewingVector();

	updateHunger();
	updateHP();

	updateGoal();
}

/**
* Subroutine of the udpate() function responsible for updating the current goal.
*/
void Wasp::updateGoal()
{
	// FOOD GOAL JUST EATEN
	if (currentGoalFoodEntity != nullptr && currentGoalFoodEntity->eaten)
	{
		currentGoalFoodEntity = nullptr;
		currentGoal = nullptr;
	}

	// RANDOM UPDATE
	if (hungerSaturation < MAX_HUNGER_SATURATION && RNG::randMod(120) == 0)
	{
		FoodEntity* food = Simulation::getFirstFoodInApproxRadius(position, 5);
		currentGoalFoodEntity = food;
		if (food != nullptr)
		{
			currentGoal = &food->position;
		}
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
	if (currentGoal != nullptr && glm::length(*currentGoal - position) < 1.5)
	{
		currentGoal = nullptr; 

		//FOOD
		if (currentGoalFoodEntity != nullptr)
		{
			onFoodReached();
		}
	}
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


//HEALTH

/**
* Kills the wasp. Until this wasp is respawned, isAlive will be false.
* Dead wasp objects are kept in memory but are no longer updated or rendered.
*/
void Wasp::kill()
{
	hp = 0;
	isAlive = false;
}

void Wasp::updateHP()
{
	if (hp <= 0)
	{
		return;
	}

	if (hungerSaturation <= 0 && RNG::randMod(20) == 0)
	{
		hp--;
	}
}


// HUNGER

void Wasp::onFoodReached()
{
	if (Simulation::attemptEatFoodMutex(currentGoalFoodEntity))
	{
		hungerSaturation += currentGoalFoodEntity->hungerPoints;

		if (hungerSaturation > 100)
		{
			hungerSaturation = 100;
		}
	}

	currentGoalFoodEntity = nullptr;
}

void Wasp::updateHunger()
{
	if (hungerSaturation <= 0)
	{
		return;
	}

	steady_clock::time_point now = steady_clock::now();
	duration<double> timeElapsed = duration_cast<std::chrono::seconds>(now - lastHungerDecrease);
	if (timeElapsed.count() >= secondsBetweenHungerDecreases)
	{
		hungerSaturation--;
		lastHungerDecrease = now;
	}
}