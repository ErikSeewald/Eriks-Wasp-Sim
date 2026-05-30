#include "Wasp.h"
#include "Queen.h"
#include "Simulation.h"
#include "RNG.h"
#include "VectorMath.h"
#include "WaspSlots.h"

/**
* Constructs the wasp object and assigns it the given w_Index (which indicates its position in the wasp vector).
* Also assigns the reference to the queen. Every wasp knows its queen.
* By default the wasp is constructed with isAlive = false.
*/
Wasp::Wasp(const int w_Index, Queen& queen) : w_Index(w_Index), queen(queen)
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
	turnSpeed = 0;
	ascendSpeed = 0;

	// GENES
	unboundGenes = WaspGenes::mutate(unboundGenes);
	balancedGenes = WaspGenes::mutate(balancedGenes);
	hp = balancedGenes.maxHP;
	hungerSaturation = balancedGenes.maxHungerSaturation;
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
	die();
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
		die();
		return;
	}
	double deltaTime = Simulation::getDeltaTime()->count();


	// --- GOAL ---
	if (currentGoalFoodEntity != nullptr && currentGoalFoodEntity->eaten)
	{
		currentGoalFoodEntity = nullptr;
		currentGoal = nullptr;
	}

	if (hungerSaturation < balancedGenes.maxHungerSaturation && RNG::randMod(50) == 1)
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

	// Highly loyal wasps will move close to the queen if she needs food
	else if (queen.hungerSaturation < 0.5 * queen.balancedGenes.maxHungerSaturation)
	{
		if (glm::distance(position, queen.position) > Queen::INTERACTION_RADIUS)
		{ 
			if (RNG::randBetween(0.0, 1.0) * unboundGenes.queenLoyalty > 0.75) { currentGoal = &queen.position; } 
		}

		else if (currentGoal == &queen.position) // No need to keep moving towards her if already in range
		{
			currentGoal = nullptr;
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
	float speedMultiplier = balancedGenes.flyingSpeed * deltaTime;
	position.x += viewingVector.x * speedMultiplier;
	position.y += viewingVector.y * speedMultiplier;
	position.z += viewingVector.z * speedMultiplier;

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

	// --- QUEEN INTERACTION ---
	if (w_Index != Queen::W_INDEX && queen.isAlive)
	{
		if (queenInteractionCountdown > 0) { queenInteractionCountdown--; }

		else
		{
			float distToQueen = glm::distance(position, queen.position);
			if (distToQueen < Queen::INTERACTION_RADIUS)
			{
				// The average wasp (loyalty 1.0) should be willing to give at most half of its food to the queen.
				// The most loyal would give her everything they have.
				giveFoodToQueen(RNG::randBetween(0, hungerSaturation * 0.5 * unboundGenes.queenLoyalty));
				queenInteractionCountdown = QUEEN_INTERACTION_TIMEOUT;
			}
		}
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

/**
* Gives the given amount of food to the queen and thereby decreases the wasp's own hungerSaturation.
* Limits the amount to the actual available amount of food.
* Does not check food availability or distance to the queen.
*/
void Wasp::giveFoodToQueen(int amount)
{
	if (amount > hungerSaturation) { amount = hungerSaturation; }

	Queen::InteractionResponse response = queen.receiveFood(amount, this->w_Index);
	if (response == Queen::InteractionResponse::Denied) { return; }

	hungerSaturation -= amount;
}

/**
* Shared death logic between all different ways that a wasp can die.
*/
inline void Wasp::die()
{
	hp = 0;
	isAlive = false;
	if (queen.isAlive) { queen.resetWorkerDossier(w_Index); }
	WaspSlots::registerDeath();
}