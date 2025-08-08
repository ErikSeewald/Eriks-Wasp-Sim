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
* Kills the wasp. Until this wasp is respawned, isAlive will be false.
* Dead wasp objects are kept in memory but are no longer updated or rendered.
*/
void Wasp::kill()
{
	hp = 0;
	isAlive = false;
}


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

/**
* Implementation/Override of the 'Updatable' class update method. Updates the wasps state in the simulation.
*/
void Wasp::update()
{
	// Note: Instead of breaking up a lot of this code into smaller, more focused functions and using the 'inline' keyword,
	// I chose to actually inline them to have tighter control, since this update logic is one of the most performance
	// critial parts of the simulation. This does come at a cost of 'cleanliness' of course, but oh well.

	if (!isAlive) { return; }
	if (hp <= 0)
	{
		isAlive = false;
		WaspSlots::registerDeath();
		return;
	}
	double deltaTime = Simulation::getDeltaTime()->count();


	// --- GOAL ---
	// Food goal has been eaten
	if (currentGoalFoodEntity != nullptr && currentGoalFoodEntity->eaten)
	{
		currentGoalFoodEntity = nullptr;
		currentGoal = nullptr;
	}

	// Random chance to check for food goal
	if (hungerSaturation < MAX_HUNGER_SATURATION && RNG::randMod(50) == 1)
	{
		// Randomly target goal. If it is closer than the current goal, switch to it.
		// Not realistic behaviour but interesting and much more performance efficient than
		// having all wasps constantly check all food.
		FoodEntity* food = Simulation::getRandomAvailableFood();
		if (food != nullptr)
		{
			if (currentGoal == nullptr || glm::length(position - food->position) < glm::length(position - *currentGoal))
			{
				currentGoalFoodEntity = food;
				currentGoal = &food->position;
			}
		}
	}


	// --- SPEEDS ---
	if (currentGoal != nullptr) { turnTowardsGoal(); }
	else { lookAroundRandomly(deltaTime); }


	// --- VIEWING VECTOR ---
	float theta = turnSpeed * deltaTime; // Rotate around y axis
	glm::vec3 tempVV = viewingVector;
	viewingVector.x = tempVV.x * cos(theta) + tempVV.z * sin(theta);
	viewingVector.z = -tempVV.x * sin(theta) + tempVV.z * cos(theta);

	viewingVector.y = ascendSpeed; // Fly up or down


	// --- POSITION ---
	float speedMultiplier = flyingSpeed * deltaTime;
	position.x += viewingVector.x * speedMultiplier;
	position.y += viewingVector.y * speedMultiplier;
	position.z += viewingVector.z * speedMultiplier;

	//Reached goal
	if (currentGoal != nullptr && glm::length(*currentGoal - position) < 1.5)
	{
		currentGoal = nullptr; 
		if (currentGoalFoodEntity != nullptr)
		{
			onFoodReached();
		}
	}


	// --- RESOURCES ---
	std::chrono::steady_clock::time_point* now = Simulation::getCachedTimePoint();

	if (*now - lastResourceTick >= RESOURCE_TICK_INTERVAL)
	{
		lastResourceTick = *now;

		if (hungerSaturation > 0) { hungerSaturation--; }
		else if (hp >0) { hp--; }
	}
}

inline void Wasp::lookAroundRandomly(double deltaTime)
{
	static const double directionSwitchSeconds = 3;
	static double secondsUntilDirectionSwitch = 0;

	secondsUntilDirectionSwitch -= deltaTime;

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

inline void Wasp::turnTowardsGoal()
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