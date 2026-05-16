#include "WaspGenes.h"

using WaspGenes::UnboundGenes;
using WaspGenes::BalancedGenes;

// Uses the BalancedGenes struct as a map.
// The value of a given gene represents its relative weight in the balancing function.
// If geneA: 1.0 and geneB: 20.0 then a change in geneB needs to be balanced by change 20x as strong in geneA.
const BalancedGenes GENE_COSTS = 
{
    .maxHP = 1.0,
    .maxHungerSaturation = 1.0,
    .flyingSpeed = 20.0
};

// Uses the BalancedGenes struct as a map.
// The value of a given gene represents its 'orientation', i.e., the direction in which a change in its value
// is considered a benefit for the wasp.
// If the value is +1.0, an increase is considered good and if the value is -1.0 a decrease is considered good.
//
// Note that there may be cases where the wasp can actually see a benefit from a mutation in the 'bad' direction.
// The value here only represents a general assumption that I made for the purposes of better balancing.
const BalancedGenes GENE_ORIENTATIONS = 
{
    .maxHP = +1.0,
    .maxHungerSaturation = +1.0,
    .flyingSpeed = +1.0
};

/**
* Takes two pointers to members of BalancedGenes and the value by which geneA should change. 
* Returns the value by which geneB should change to balance it out.
*/
constexpr float balanceGeneChange(float BalancedGenes::* geneA, float changeA, float BalancedGenes::* geneB)
{
    float balance = (GENE_COSTS.*geneA) / (GENE_COSTS.*geneB);

    // If A increases:
    // A+ & B+ -> Good change for A -> Need bad change for B  -> -
    // A- & B- -> Bad change for A  -> Need good change for B -> -
    // A+ & B- -> Good change for A -> Need bad change for B  -> +
    // A- & B+ -> Bad change for A  -> Need good change for B -> +
    float orientation = -((GENE_ORIENTATIONS.*geneA) * (GENE_ORIENTATIONS.*geneB));

    return changeA * balance * orientation;
};

/**
* Mutates the given BalancedGenes randomly and returns a new set of genes.
* Handles the weighted trade-off between genes to balance the result.
* Growing/decreasing gene values in favor of the wasp means a sacrifice needs to be made elsewhere.
*/
BalancedGenes WaspGenes::mutate(BalancedGenes genes)
{
    // TODO
}