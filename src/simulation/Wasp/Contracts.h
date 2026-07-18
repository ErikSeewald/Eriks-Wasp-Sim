#pragma once

#include <vector>
#include <string>
#include <chrono>
#include <mutex>
#include "Food.h"

// Forward declarations
class Wasp;

/**
* @namespace Contratcs
* @brief Namespace for contracts that can be formed between wasps.
*/
namespace Contracts
{

    std::mutex* getContractMutex();

    /**
    * All contracts use a shared timer for their duration calculations. This function updates it.
    * I chose against using std::chrono::time_point to make this work better with pausing and debugging.
    */
    void updateContractTimer(std::chrono::duration<double>* deltaTime);

    /**
    * The implementation file for the Contracts namespace has static ownership of all exisiting contracts.
    * Therefore, it must also be the one to clean up expired contracts and remove references to them.
    * This function should be run periodically.
    */
    void cleanupExpiredContracts();

    /**
    * Returns the number of active contracts as was determined during the last run of cleanupExpiredContracts().
    * May be out of date.
    */
    int getNumActiveContracts();
    
    /**
    * @enum ContractType
    * @brief Enum specifying all possible contract types. Used with Contract::getType().
    */
    enum ContractType
    { 
        FoodSharingContractType, CliqueContractType
    };

    /**
    * Returns the c_str representation of the given ContractType.
    */
    const char* contractTypeAsStr(ContractType type);

    /**
    * @class Contract
    * @brief A class servering as the base contract type. Tracks remaining validity and a list of partners.
    */
    class Contract 
    {
        public:

            /**
            * Creates a contract between the two given partners that is valid for the given amount of seconds.
            * The partners need to be different wasps. Exits the program with an error otherwise.
            * Additional partners need to be added later. 
            * Gives ownership of this object to a static list in the Contracts namespace for memory management.
            */
            Contract(Wasp* partner1, Wasp* partner2, double validForSeconds);

            /**
             * Returns the type of the contract.
             */
            virtual ContractType getType() const = 0;
            
            /**
             * Allows two partners that have agreed on forming a contract to negotiate its terms, thereby
             * creating the contract to which a pointer is then returned.
             */
            static Contract* negotiateTerms(ContractType type, Wasp* partner1, Wasp* partner2);

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
            * Returns true if the validity period of this contract is still active and
            * there are at least 2 living partners left.
            * 
            * If the contract has become invalid, it is the responsibility of the first entity that notices
            * to free the memory and the responsibility of all others to always check for nullptr.
            */
            bool isValid();

            /**
             * Allows immediate invalidation of the contract.
             */
            void invalidate();

            /**
             * Registers the death of one of the partners in the contract. Critical for ensuring
             * contracts get invalidated and deleted when less than two living partners remain.
             */
            void registerPartnerDeath(Wasp* partner);

            /**
            * Returns the list of all partners involved in this contract.
            */
            const std::vector<Wasp*>& getPartners();

            /*
            * Adds the given wasp to the list of partners.
            * Exits the program with an error if the given partner is nullptr or already in the list. 
            */
            void addPartner(Wasp* partner);

        protected:
            std::vector<Wasp*> partners;

        private:
            
            const double validityStart; // Value of the global contract timer at the start of validity
            double validityEnd; // Value of the global contract timer at the end of validity   
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

            ContractType getType() const override { return ContractType::FoodSharingContractType; }

            FoodSharingContract(
                Wasp* partner1, Wasp* partner2, double validForSeconds,
                int hungerSaturationAllowance, float sharingRate
            ) : Contract(partner1, partner2, validForSeconds), 
                hungerSaturationAllowance(hungerSaturationAllowance), sharingRate(sharingRate) {}

            /**
            * Type-specific implementation of Contract::negotiateTerms().
            */
            static Contract* negotiateTermsImpl(Wasp* partner1, Wasp* partner2);

            /**
             * If the given initiator has eaten the given food, this function handles
             * the distribution of the attained food points amongst the partners.
             */
            void handleFoodEncounter(Wasp* initiator, Food::FoodEntity* food);
    };

    /**
    * @class CliqueContract
    * @brief A contract that requires all of the involved partners to stay within a certain range of partner 1
    *        (the first partner in the partners array - can change depending on deaths, etc.).
    */
    class CliqueContract : public Contract
    {
        public:

            // When being futher away from partner 1 than this range, a partner must set its current goal
            // to be partner 1.
            const double range;

            ContractType getType() const override { return ContractType::CliqueContractType; }

            CliqueContract(
                Wasp* partner1, Wasp* partner2, double validForSeconds, double range
            ) : Contract(partner1, partner2, validForSeconds), range(range) {}

            /**
            * Type-specific implementation of Contract::negotiateTerms().
            */
            static Contract* negotiateTermsImpl(Wasp* partner1, Wasp* partner2);

            /**
             * If the given wasp is out of range of partner 1, this function overrides its current
             * goal with the position of partner 1. Does nothing otherwise.
             */
            void overrideGoalIfOutOfRange(Wasp* partner);
    };
};

