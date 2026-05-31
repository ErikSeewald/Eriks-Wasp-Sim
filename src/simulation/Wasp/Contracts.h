#pragma once

#include <vector>

class Wasp; // Forward declaration so the wasp can be referenced here

/**
* @namespace Contratcs
* @brief Namespace for contracts that can be formed between wasps.
*/
namespace Contracts
{
    /**
    * The implementation file for the Contracts namespace has static ownership of all exisiting contracts.
    * Therefore, it must also be the one to clean up expired contracts and remove references to them.
    * This function should be run periodically.
    */
    void cleanupExpiredContracts();

    /**
    * @class Contract
    * @brief A class servering as the base contract type. Tracks remaining validity and a list of partners.
    */
    class Contract 
    {
        public:

            /**
            * Creates a contract between the two given partners that is valid for the given amount of seconds.
            * Additional partners need to be added later. 
            * Gives ownership of this object to a static list in the Contracts namespace for memory management.
            */
            Contract(Wasp* partner1, Wasp* partner2, double validForSeconds);

            /**
            * Returns the amount of seconds left until this contract expires.
            */
            double getRemainingValiditySeconds();

            /**
            * Extends the validity of the contract by the given amount of seconds
            * ONLY IF the contract is still valid. 
            * 
            * Throws a runtime error if contract has expired or the given amount of
            * seconds is negative.
            */
            void extendValidityBySeconds( double seconds);

            /**
            * Returns whether the validity period of this contract is still active.
            * 
            * If the contract has become invalid, it is the responsibility of the first entity that notices
            * to free the memory and the responsibility of all others to always check for nullptr.
            */
            bool isValid();

            /**
            * Returns the list of all partners involved in this contract.
            */
            const std::vector<Wasp*>& getPartners();

        private:
            const double initialValiditySeconds;
            double remainingValiditySeconds;

            std::vector<Wasp*> partners;
    };

    /**
    * @class FoodSharingContract
    * @brief A contract that stipulates a minimum level of hunger saturation (allowance) beyond which a certain percentage 
    *        of acquired food is shared between all partners.
    */
    class FoodSharingContract : public Contract
    {
        public:

            // If the wasp has a hungerSaturation below this allowance, it does not need to share any of its acquired food.
            const int hungerSaturationAllowance;

            // The relative amount of any new chunk of acquired food that needs to be shared.
            const float sharingRate;

            FoodSharingContract(
                Wasp* partner1, Wasp* partner2, double validForSeconds,
                int hungerSaturationAllowance, float sharingRate
            ) : Contract(partner1, partner2, validForSeconds), 
                hungerSaturationAllowance(hungerSaturationAllowance), sharingRate(sharingRate) {}
    };
};