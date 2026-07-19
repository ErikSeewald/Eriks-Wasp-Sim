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
		for (int i = 0; i < Wasp::MAX_NUM_CONTRACTS; i++)
		{
			if (contracts[i] != nullptr && contracts[i]->getType() == Contracts::ContractType::FoodSharingContractType)
			{
				Contracts::FoodSharingContract* fsc = (Contracts::FoodSharingContract*) contracts[i];
				fsc->handleFoodEncounter(this, currentGoalFoodEntity);
				break;
			}
		}

		int _leftovers = addHungerSaturationBounded(currentGoalFoodEntity->hungerPoints);
		// In this case the leftovers are just discarded
	}

	currentGoalFoodEntity = nullptr;
}

/**
* Adds the given amount of points to the wasp's hunger saturation
* while limiting itself to its maxHungerSaturation.
* Returns the amount of points that could not be consumed due to
* going over the maximum.
*/
int Wasp::addHungerSaturationBounded(int saturationPoints)
{
	hungerSaturation += saturationPoints;

	if (hungerSaturation > balancedGenes.maxHungerSaturation)
	{
		int leftovers = hungerSaturation - balancedGenes.maxHungerSaturation;
		hungerSaturation = balancedGenes.maxHungerSaturation;
		return leftovers;
	}
	return 0;
}

/**
*  Sets the 'privileged' state of the wasp, dictating whether it is allowed to perform certain expensive computations.
*/ 
void Wasp::setPrivileged(bool privileged)
{
	isPrivileged = privileged;
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

	// Override the goal while the wasp is out of range in a SwarmContract
	for (int i = 0; i < Wasp::MAX_NUM_CONTRACTS; i++)
	{
		if (contracts[i] != nullptr && contracts[i]->getType() == Contracts::ContractType::SwarmContractType)
		{
			((Contracts::SwarmContract*) contracts[i])->overrideGoalIfOutOfRange(this);
			break;
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

	// --- CONTRACTS ---
	tryProposeContract(deltaTime);

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
* Allows the given wasp to propose a contract of the given type to this wasp.
* Performs the terms negotiation and creates the contract if the proposal was accepted.
* Returns a pointer to the newly created contract or nullptr if the proposal was rejected.
*/
Contracts::Contract* Wasp::receiveNewContractProposal(Wasp* proposer, Contracts::ContractType type)
{
	int contractIndex = getAvailableContractIndex();
	if (contractIndex == -1) { return nullptr; }

	float interestFactor = 1.0; // No special factor for a new contract.
	bool accepted = considerAcceptingContract(proposer, type, interestFactor);
	if (!accepted) { return nullptr; }

	// Yes, term negotiations take place after both parties have already agreed to form a contract.
	// This is how wasps do it.
	Contracts::Contract* newContract = Contracts::Contract::negotiateTerms(type, this, proposer);
	contracts[contractIndex] = newContract;

	return newContract;
}


/**
* Allows the given wasp to propose joining the given contract to this wasp.
* Returns true if that proposal was accepted. In that case,
* the function already adds this wasp to the list of partners.
*/
bool Wasp::receiveContractJoinProposal(Wasp* proposer, Contracts::Contract* contract)
{
	int contractIndex = getAvailableContractIndex();
	if (contractIndex == -1) { return false; }

	// Scale by how many wasps are already in the contract.
	// Contracts with larger partner counts have "proven" themselves to be worthwhile.
	// NOTE: Scaling function falls off in intensity with increasing partner counts
	// so that it never becomes a guaranteed accept response.
	float interestFactor = 1.0 + 5.0 * log(contract->getPartners().size());

	bool accepted = considerAcceptingContract(proposer, contract->getType(), interestFactor);
	if (!accepted) { return false; }

	contracts[contractIndex] = contract;
	contract->addPartner(this);
	return true;
}

/**
* Considers interest in the contract based on type-specific conditions and returns
* whether the wasp should accept (true) or reject (false) the proposal.
* The given interestFactor can be used to scale the base interest in the contract.
* Note: Checking whether an available slot for the contract is even free is the
* responsibility of the caller.
*/
bool Wasp::considerAcceptingContract(Wasp* proposer, Contracts::ContractType type, float interestFactor)
{
	if (getContractIndexByType(type) != -1) { return false;} // Max. 1 contract per type.

	// A base likelihood to accept the contract is generated and can then be
	// updated by further conditions before the final outcome is determined.
	float interest = RNG::randBetween(0.0, 1.0);
	interest *= interestFactor;

	switch (type)
	{
		case Contracts::ContractType::FoodSharingContractType:
			// Scale interest by how much food the proposer has in relation to one self
			if (hungerSaturation < 1) { interest = INFINITY;}
			else {interest *= (float) proposer->hungerSaturation / (float) hungerSaturation; }
			break;
		case Contracts::ContractType::SwarmContractType:
		 	// Wasps have a higher base interest in swarm contracts because the don't give up much for it
			interest *= 3.0;
			break;
	}

	const float ACCEPTANCE_THRESHOLD = 0.5;
	return interest * unboundGenes.contractDesire >= ACCEPTANCE_THRESHOLD;
}

/**
 * By random chance the wasp can try to propose a contract to another wasp.
 */
void Wasp::tryProposeContract(double deltaTime)
{
	const double SECONDS_BETWEEN_PROPOSALS = 5.0;
	static double timeSinceLastProposal = SECONDS_BETWEEN_PROPOSALS;
	timeSinceLastProposal += deltaTime;
	if (timeSinceLastProposal < SECONDS_BETWEEN_PROPOSALS) { return; }

	// If a contract index is available, the wasp has a random chance of wanting to propose one.
	if (RNG::randBetween(0.0, 1.0) > unboundGenes.contractDesire) { return; }

	// The lock only needs to be acquired here because all contract creation functions will be
	// called from the same thread (e.g., considerAcceptingContract).
	std::lock_guard<std::mutex> lock(*Contracts::getContractMutex());

	// Chooses a random wasp to propose to. Finding one in range would be too much of a toll on performance.
	// Swarm contracts should take care of interesting range behavior anyway.
    std::vector<Wasp>* wasps = WaspSlots::getWasps();
    int maxIndex = WaspSlots::getMaxIndex();
	if (maxIndex < 1) { return; }
	Wasp* partner = &(*wasps)[RNG::randMod(maxIndex)];

	// Treat these as another random chance for not proposing a contract.
	if (!partner->isAlive || partner == this) { return; }

	Contracts::ContractType type = RNG::randMod(2) == 1 ? 
			Contracts::ContractType::FoodSharingContractType : Contracts::ContractType::SwarmContractType;
	int contractIndex = getContractIndexByType(type);

	// Propose adding the partner to the existing contract
	if (contractIndex != -1) 
	{ 
		bool _accepted = partner->receiveContractJoinProposal(this, contracts[contractIndex]);
	}

	// Propose a new contract with the partner
	else
	{
		contractIndex = getAvailableContractIndex();
		if (contractIndex == -1) { return; }

		Contracts::Contract* newContract = partner->receiveNewContractProposal(this, type);
		if (newContract != nullptr)
		{
			contracts[contractIndex] = newContract;
		}
	}

	timeSinceLastProposal = 0.0;
}

/**
* Returns an index of a contract slot that is available to be overwritten (is nullptr or expired).
* Returns -1 if no such index could be found. 
*/
int Wasp::getAvailableContractIndex()
{
	for (int i = 0; i < Wasp::MAX_NUM_CONTRACTS; i++)
	{
		if (contracts[i] == nullptr || !contracts[i]->isValid()) 
		{ 
			return i;
		}
	}
	return -1;
}


/**
* Wasps are only allowed a maximum of one active contract per type. Therefore,
* this function either returns the index of that contract or -1 if the wasp
* does not yet have a (still active) contract of the given type.
*/
int Wasp::getContractIndexByType(Contracts::ContractType type)
{
	for (int i = 0; i < Wasp::MAX_NUM_CONTRACTS; i++)
	{
		if (contracts[i] != nullptr && contracts[i]->isValid() && contracts[i]->getType() == type)
		{
			return i;
		}
	}
	return -1;
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

	for (int i = 0; i < Wasp::MAX_NUM_CONTRACTS; i++)
	{
		if (contracts[i] != nullptr) 
		{ 
			contracts[i]->registerPartnerDeath(this);
			contracts[i] = nullptr;
		}
	}

	if (queen.isAlive) { queen.resetWorkerDossier(w_Index); }
	WaspSlots::registerDeath();
}