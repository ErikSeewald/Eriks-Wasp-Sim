#include "Contracts.h"
#include <stdexcept>

using namespace Contracts;


/**
* Creates a contract between the two given partners that is valid for the given amount of seconds.
* Additional partners need to be added later. 
*/
Contract::Contract(const Wasp& partner1, const Wasp& partner2, double validForSeconds) : initialValiditySeconds(validForSeconds)
{
    partners.push_back(partner1);
    partners.push_back(partner2);

    remainingValiditySeconds = validForSeconds;
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
