#include "Contracts.h"
#include "Wasp.h"
#include <stdexcept>
#include <algorithm>


using Contracts::Contract;

std::vector<Contract*> _activeContracts;

/**
* The implementation file for the Contracts namespace has static ownership of all exisiting contracts.
* Therefore, it must also be the one to clean up expired contracts and remove references to them.
* This function should be run periodically.
*/
void Contracts::cleanupExpiredContracts()
{
    for (int i = 0; i < _activeContracts.size(); i++)
    {
        if (_activeContracts[i]->isValid()) { continue; }

        // Remove the pointer from all wasps that were partners in the contract
        const std::vector<Wasp*>& partners = _activeContracts[i]->getPartners();
        for (Wasp* partner : partners)
        {
            for (int j = 0; j < Wasp::MAX_NUM_CONTRACTS; j++)
            {
                if (partner->contracts[j] == _activeContracts[i])
                {
                    partner->contracts[j] == nullptr;
                    break;
                }
            }
        }

        delete _activeContracts[i];
        _activeContracts[i] = nullptr;
    }

    // Remove nullptrs
    _activeContracts.erase(std::remove(_activeContracts.begin(), _activeContracts.end(), nullptr), _activeContracts.end());
}

/**
* Registers the given contract in the list of active contracts by either
* intersting it at the first nullptr it finds or extending the vector.
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


//-------------------------------------
//---------------CONTRACT--------------
//-------------------------------------
/**
* Creates a contract between the two given partners that is valid for the given amount of seconds.
* Additional partners need to be added later. 
* Gives ownership of this object to a static list in the Contracts namespace for memory management.
*/
Contract::Contract(Wasp* partner1, Wasp* partner2, double validForSeconds) : initialValiditySeconds(validForSeconds)
{
    partners.push_back(partner1);
    partners.push_back(partner2);

    remainingValiditySeconds = validForSeconds;

    _activeContracts.push_back(this);
}

/**
* Returns the amount of seconds left until this contract expires.
*/
double Contract::getRemainingValiditySeconds()
{
    return remainingValiditySeconds;
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

    if (seconds > 0.0)
    {
        throw std::runtime_error("Cannot extend a contract by < 0 seconds");
    }

    remainingValiditySeconds += seconds;
}

/**
* Returns whether the validity period of this contract is still active.
*/
bool Contract::isValid()
{
    return remainingValiditySeconds > 0.0;
}

/**
* Returns the list of all partners involved in this contract.
*/
const std::vector<Wasp*>& Contract::getPartners()
{
    return partners;
}


//-------------------------------------
//--------FOOD SHARING CONTRACT--------
//-------------------------------------

