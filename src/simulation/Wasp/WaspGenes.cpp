#include "WaspGenes.h"
#include "RNG.h"
#include <array>

using WaspGenes::UnboundGenes;
using WaspGenes::BalancedGenes;

using BGenePointer = float BalancedGenes::*;
const int NUM_B_GENE_POINTERS = 3;

// Vector of pointers to all attributes of BalancedGenes to use for non-named access.
// Leaving a pointer out of this vector means it will not be considered by functions like mutate().
constexpr std::array<BGenePointer, NUM_B_GENE_POINTERS> B_GENE_POINTERS =
{
    &BalancedGenes::maxHP,
    &BalancedGenes::maxHungerSaturation,
    &BalancedGenes::flyingSpeed
};


// Uses the BalancedGenes struct as a map.
// The value of a given gene represents its relative weight in the balancing function.
// If geneA: 1.0 and geneB: 20.0 then a change in geneB needs to be balanced by change 20x as strong in geneA.
const BalancedGenes B_GENE_COSTS = 
{
    .maxHP = 1.0,
    .maxHungerSaturation = 1.0,
    .flyingSpeed = 50.0
};

// Uses the BalancedGenes struct as a map.
// The value of a given gene represents its 'orientation', i.e., the direction in which a change in its value
// is considered a benefit for the wasp.
// If the value is +1.0, an increase is considered good and if the value is -1.0 a decrease is considered good.
//
// Note that there may be cases where the wasp can actually see a benefit from a mutation in the 'bad' direction.
// The value here only represents a general assumption that I made for the purposes of better balancing.
const BalancedGenes B_GENE_ORIENTATIONS = 
{
    .maxHP = +1.0,
    .maxHungerSaturation = +1.0,
    .flyingSpeed = +1.0
};

/**
* Takes two pointers to members of BalancedGenes and the value by which geneA should change. 
* Returns the value by which geneB should change to balance it out.
*/
float balanceGeneMutation(BGenePointer geneA, float changeA, BGenePointer geneB)
{
    float balance = (B_GENE_COSTS.*geneA) / (B_GENE_COSTS.*geneB);

    // If A increases:
    // A+ & B+ -> Good change for A -> Need bad change for B  -> -
    // A- & B- -> Bad change for A  -> Need good change for B -> -
    // A+ & B- -> Good change for A -> Need bad change for B  -> +
    // A- & B+ -> Bad change for A  -> Need good change for B -> +
    float orientation = -((B_GENE_ORIENTATIONS.*geneA) * (B_GENE_ORIENTATIONS.*geneB));

    return changeA * balance * orientation;
};

/**
* Mutates the given BalancedGenes randomly and returns a new set of genes.
* Handles the weighted trade-off between genes to balance the result.
* Growing/decreasing gene values in favor of the wasp means a sacrifice needs to be made elsewhere.
*/
BalancedGenes WaspGenes::mutate(BalancedGenes genes)
{
    static const float DEFAULT_MUTATION_RANGE = 10.0; // A gene with weight 1.0 can mutate in this range

    static constexpr int MAX_MUTATION_ROUNDS = 8 * NUM_B_GENE_POINTERS;
    int mutationRounds = RNG::randMod(MAX_MUTATION_ROUNDS);

    BalancedGenes mutated = genes;
    for (int i = 0; i < mutationRounds; i++)
    {
        int mutateTargetIdx = RNG::randMod(NUM_B_GENE_POINTERS);
        BGenePointer mutateTarget = B_GENE_POINTERS[mutateTargetIdx];

        float mutationRange = DEFAULT_MUTATION_RANGE /  B_GENE_COSTS.*mutateTarget;
        float mutation = RNG::randBetween(-mutationRange, mutationRange);

        int balancingTargetIdx = RNG::randMod(NUM_B_GENE_POINTERS);
        if (balancingTargetIdx == mutateTargetIdx) { continue; } // Treat this as the random chance for no mutation taking place
        BGenePointer balancingTarget = B_GENE_POINTERS[balancingTargetIdx];

        float balanceMutation = balanceGeneMutation(mutateTarget, mutation, balancingTarget);
        mutated.*mutateTarget += mutation;
        mutated.*balancingTarget += balanceMutation;
    }
    return mutated;
}