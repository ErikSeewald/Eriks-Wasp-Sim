#include "Wasp.h"
#include <vector>

/**
* @namespace Contratcs
* @brief Namespace for contracts that can be formed between wasps.
*/
namespace Contracts
{
    /**
    * @class Contract
    * @brief An abstract class servering as the base contract type. Tracks remaining validity and a list of partners.
    */
    class Contract 
    {
        public:

            /**
            * Creates a contract between the two given partners that is valid for the given amount of seconds.
            * Additional partners need to be added later. 
            */
            Contract(const Wasp& partner1, const Wasp& partner2, double validForSeconds);

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
            */
            bool isValid();

            /**
            * Returns the list of all partners involved in this contract.
            */
            const std::vector<const Wasp&>& getPartners();

        private:
            const double initialValiditySeconds;
            double remainingValiditySeconds;

            std::vector<const Wasp&> partners;
    };
}