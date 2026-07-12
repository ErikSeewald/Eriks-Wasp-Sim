#include "Contracts.h"
#include "Wasp.h"
#include "RNG.h"
#include "UI.h"
#include <stdexcept>
#include <algorithm>
#include <iostream>


using Contracts::Contract;
using Contracts::ContractType;

std::vector<Contract*> _activeContracts;

// Should be fine as long as noone leaves this sim running for a million years.
double _contractTimerSeconds;

/**
* All contracts use a shared timer for their duration calculations. This function updates it.
* I chose against using std::chrono::time_point to make this work better with pausing and debugging.
*/
void Contracts::updateContractTimer(std::chrono::duration<double>* deltaTime)
{
    _contractTimerSeconds += deltaTime->count();
}

/**
* Returns the number of active contracts as was determined during the last run of cleanupExpiredContracts().
* May be out of date.
*/
int Contracts::getNumActiveContracts()
{
    return _activeContracts.size();
}

/**
* The implementation file for the Contracts namespace has static ownership of all exisiting contracts.
* Therefore, it must also be the one to clean up expired contracts and remove references to them.
* This function should be run periodically.
*/
void Contracts::cleanupExpiredContracts()
{
    UI::UI_STATE* uiState = UI::getUIState();

    for (int i = 0; i < _activeContracts.size(); i++)
    {
        if (_activeContracts[i]->isValid()) 
        { 
            if (_activeContracts[i]->getPartners().size() < 2) 
            {
                _activeContracts[i]->invalidate(); // Needs to go too!
            }
            else { continue; } // Can stay :)
        }

        // Remove the pointer from all wasps that were partners in the contract
        const std::vector<Wasp*>& partners = _activeContracts[i]->getPartners();
        for (Wasp* partner : partners)
        {
            for (int j = 0; j < Wasp::MAX_NUM_CONTRACTS; j++)
            {
                if (partner->contracts[j] == _activeContracts[i])
                {
                    partner->contracts[j] = nullptr;
                    break;
                }
            }
        }

        // Potentially remove the pointer from uiState
        if (uiState->selectedContract == _activeContracts[i])
        { uiState->selectedContract = nullptr; }

        delete _activeContracts[i];
        _activeContracts[i] = nullptr;
    }

    // Remove nullptrs
    _activeContracts.erase(std::remove(_activeContracts.begin(), _activeContracts.end(), nullptr), _activeContracts.end());
}

/**
* Registers the given contract in the list of active contracts by either
* inserting it at the first nullptr it finds or extending the vector.
*/
void _registerActiveContract(Contract* contract)
{
    for (int i = 0; i < _activeContracts.size(); i++)
    {
        if (_activeContracts[i] == nullptr)
        {
            _activeContracts[i] = contract;
            return;
        }
    }

    _activeContracts.push_back(contract);
}

/**
* Returns the c_str representation of the given ContractType.
*/
const char* Contracts::contractTypeAsStr(ContractType type)
{
    switch (type)
    {
        case ContractType::FoodSharingContractType:
            return "FoodSharingContract";
        case ContractType::CliqueContractType:
            return "CliqueContract";
        default:
            return "UNKNOWN";
    }
}


//-------------------------------------
//---------------CONTRACT--------------
//-------------------------------------
/**
* Creates a contract between the two given partners that is valid for the given amount of seconds.
* The partners need to be different wasps. Exits the program with an error otherwise.
* Additional partners need to be added later. 
* Gives ownership of this object to a static list in the Contracts namespace for memory management.
*/
Contract::Contract(Wasp* partner1, Wasp* partner2, double validForSeconds) : validityStart(_contractTimerSeconds)
{
    if (partner1 == partner2) 
    { 
        std::cerr << "Tried to create contract where partner1 and partner2 are both " << partner1 << std::endl;
        exit(EXIT_FAILURE);
    }

    partners.push_back(partner1);
    partners.push_back(partner2);

    validityEnd = validityStart + validForSeconds;

    // This causes undefined behaviour if a contract is ever created on the stack.
    // But right now I am not really into the idea of enforcing heap-only creation. Oh well.
    _registerActiveContract(this);
}

/**
 * Allows two partners that have agreed on forming a contract to negotiate its terms, thereby
 * creating the contract to which a pointer is then returned.
 */
Contract* Contract::negotiateTerms(ContractType type, Wasp* partner1, Wasp* partner2)
{
    // I am doing this instead of overrides of the negtiateTerms function because I want
    // all the contract decision code prior to work on 'ContractType' and only map
    // to the actual class for that type at this point.
    switch (type)
    {
        case ContractType::FoodSharingContractType:
            return FoodSharingContract::negotiateTermsImpl(partner1, partner2);
        case ContractType::CliqueContractType:
            return CliqueContract::negotiateTermsImpl(partner1, partner2);
    }
    return nullptr;
}

/**
* Returns the amount of seconds left until this contract expires.
*/
double Contract::getRemainingValiditySeconds()
{
    return validityEnd - _contractTimerSeconds;
}

/**
* Extends the validity of the contract by the given amount of seconds
* ONLY IF the contract is still valid. 
* 
* Throws a runtime error if contract has expired or the given amount of
* seconds is negative.
*/
void Contract::extendValidityBySeconds(double seconds)
{
    if (!isValid())
    {
        throw std::runtime_error("Cannot extend expired contract");
    }

    if (seconds < 0.0)
    {
        throw std::runtime_error("Cannot extend a contract by < 0 seconds");
    }

    validityEnd += seconds;
}

/**
* Returns true if the validity period of this contract is still active and
* there are at least 2 living partners left.
* 
* If the contract has become invalid, it is the responsibility of the first entity that notices
* to free the memory and the responsibility of all others to always check for nullptr.
*/
bool Contract::isValid()
{
    return _contractTimerSeconds < validityEnd && partners.size() >= 2;
}

/**
 * Allows immediate invalidation of the contract.
 */
void Contract::invalidate()
{
    validityEnd = _contractTimerSeconds;
}

/**
 * Registers the death of one of the partners in the contract. Critical for ensuring
 * contracts get invalidated and deleted when less than two living partners remain.
 */
void Contract::registerPartnerDeath(Wasp* partner)
{
    partners.erase(
        std::remove(partners.begin(), partners.end(), partner),
        partners.end()
    );
}


/**
* Returns the list of all partners involved in this contract.
*/
const std::vector<Wasp*>& Contract::getPartners()
{
    return partners;
}

/*
* Adds the given wasp to the list of partners.
* Exits the program with an error if the given partner is nullptr or already in the list. 
*/
void Contract::addPartner(Wasp* partner)
{
    if (partner == nullptr) 
    { 
        std::cerr << "Tried to add nullptr to contract partners" << std::endl;
        exit(EXIT_FAILURE);
    }

    for (Wasp* p : partners)
    {
        if (p == partner)
        {
            std::cerr << "Tried to add the same partner to a contract twice: " << partner << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    partners.push_back(partner);
}


//-------------------------------------
//--------FOOD SHARING CONTRACT--------
//-------------------------------------
using Contracts::FoodSharingContract;

/**
* Type-specific implementation of Contract::negotiateTerms().
*/
Contract* FoodSharingContract::negotiateTermsImpl(Wasp* partner1, Wasp* partner2)
{
    // TODO: More interesting negotiation

    // VALID FOR SECONDS
    double validForSeconds = RNG::randBetween(0.0, 200.0);

    // HUNGER SATURATION ALLOWANCE
    int smallerMaxSaturation = std::min(partner1->balancedGenes.maxHungerSaturation, partner2->balancedGenes.maxHungerSaturation);
    int hungerSaturationAllowance = RNG::randMod(smallerMaxSaturation);

    // SHARING RATE
    float sharingRate = RNG::randBetween(0.5, 1.0);

    return new FoodSharingContract(partner1, partner2, validForSeconds, hungerSaturationAllowance, sharingRate);
}

/**
 * If the given initiator has eaten the given food, this function handles
 * the distribution of the attained food points amongst the partners.
 */
void FoodSharingContract::handleFoodEncounter(Wasp* initiator, Food::FoodEntity* food)
{
    if (!isValid()) { return; }

    // Since a wasp can only ever have one FoodSharingContract, all the food can be divided
    // amongst the partners of this specific contract without worry.

    // Deal with hungerSaturationAllowance first
    if (initiator->hungerSaturation < hungerSaturationAllowance)
    {
        int foodToFillAllowance = hungerSaturationAllowance - initiator->hungerSaturation;
        int payout = std::min(food->hungerPoints, foodToFillAllowance);
        int leftovers = initiator->addHungerSaturationBounded(payout);
        food->hungerPoints -= (payout - leftovers);
    }

    // Then divide up food beyond the allowance
    int amountToShare = food->hungerPoints * sharingRate;
    int leftovers = initiator->addHungerSaturationBounded(food->hungerPoints - amountToShare);
    food->hungerPoints = amountToShare + leftovers;

    int receivers = partners.size() - 1;
    int amountForEach = receivers > 0 ? food->hungerPoints / receivers : 0;

    for (Wasp* partner : partners)
    {
        if (partner == initiator) { continue; }

        int leftovers = partner->addHungerSaturationBounded(amountForEach);
        food->hungerPoints -= (amountForEach - leftovers);
        
    }

    // Everything that is left in food->hungerPoints at this point can be eaten by the initiator
    // after the end of this function.
}


//-------------------------------------
//----------CLIQUE CONTRACT------------
//-------------------------------------
using Contracts::CliqueContract;

/**
* Type-specific implementation of Contract::negotiateTerms().
*/
Contract* CliqueContract::negotiateTermsImpl(Wasp* partner1, Wasp* partner2)
{
    // TODO: More interesting negotiation

    // VALID FOR SECONDS
    double validForSeconds = RNG::randBetween(0.0, 200.0);

    // RANGE
    float range = RNG::randBetween(2.0, 20.0);

    return new CliqueContract(partner1, partner2, validForSeconds, range);
}

/**
 * If the given wasp is out of range of partner 1, this function overrides its current
 * goal with the position of partner 1. Does nothing otherwise.
 */
void CliqueContract::overrideGoalIfOutOfRange(Wasp* partner)
{
    if (!isValid()) { return; }

    Wasp* p1 = partners.at(0);

    float distance = glm::distance(p1->position, partner->position);
    if (distance > range)
    {
        partner->currentGoal = &p1->position;
    }
}