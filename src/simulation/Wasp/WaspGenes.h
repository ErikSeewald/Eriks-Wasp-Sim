#pragma once

/**
* @namespace WaspGenes
* @brief Namespace for structs that define collections of wasp traits that can be interpreted as genes a single wasp posesses.
*/
namespace WaspGenes
{
    /**
    * @struct UnboundGenes
    * @brief A collection of traits posessed by a specific wasp. These genes are 'unbound' because they can be mutated without trade-off in another gene.
    */
    struct UnboundGenes
    {
        // Queen loyalty affects how devoted the wasp is to its queen:
        // - How likely is it to choose to fly toward the queen
        // - How much food is it willing to gift the queen
        float queenLoyalty = 1.0;
    };

    /**
    * Mutates the given Unbound randomly and returns a new set of genes.
    */
    UnboundGenes mutate(UnboundGenes genes);



    /**
    * @struct BalancedGenes
    * @brief A collection of traits posessed by a specific wasp. These genes are 'balanced' because they can be mutated only through a trade-off mutation in another gene.
    */
    struct BalancedGenes
    {
        float maxHP = 100.0;
        float maxHungerSaturation = 100.0;
        float flyingSpeed = 0.8;
    };

    /**
    * Mutates the given BalancedGenes randomly and returns a new set of genes.
    * Handles the weighted trade-off between genes to balance the result.
    * Growing/decreasing gene values in favor of the wasp means a sacrifice needs to be made elsewhere.
    */
    BalancedGenes mutate(BalancedGenes genes);
}