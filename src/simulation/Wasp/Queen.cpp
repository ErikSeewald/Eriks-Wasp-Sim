#include "Queen.h"
#include "WaspSlots.h"
#include <stdexcept>
#include <algorithm>
#include <numeric>

Queen::Queen() : _allWasps(*WaspSlots::getWasps()), Wasp(W_INDEX, *this) // The queen has no queen.. But she is her own queen.
{
    // All workers start with a score of 0
    for (int i = 0; i < WaspSlots::getSlotCount(); i++)
    {
        workerDossiers.emplace_back(WorkerDossier{i, 0});
    }

    wIndexSortedByWorkerScore.resize(WaspSlots::getSlotCount());
    std::iota(wIndexSortedByWorkerScore.begin(), wIndexSortedByWorkerScore.end(), 0); // Fill with the wasp indices
}; 

/**
* Does queen-specific updating and then calls Wasp::update()
*/
void Queen::update(const std::chrono::duration<double>& deltaTime)
{
    interactionsLeft = MAX_INTERACTIONS_PER_ITERATION;

    isHandoutDue = false;
    secondsSinceLastHandout += deltaTime.count();
    if (secondsSinceLastHandout > SECONDS_BETWEEN_HANDOUTS)
    {
        isHandoutDue = true;
        secondsSinceLastHandout = 0.0;
        sortIndicesByWorkerScore();
    }

    updateFoodStorage();
    Wasp::update();
}

/**
* Manages how the queen's stored food should be used in the current update cycle.
* Either for her own benefit or for that of her most appreciated workers.
*/
void Queen::updateFoodStorage()
{
    // If the queen's hunger saturation falls below this threshold, she will prioritize feeding herself
    if (hungerSaturation < 0.75 * balancedGenes.maxHungerSaturation)
    {
        int change = std::min((int) balancedGenes.maxHungerSaturation - hungerSaturation, hungerSaturation);
        change = std::min(storedFoodAmount, change);

        storedFoodAmount -= change;
        hungerSaturation += change;
    }

    // Otherwise, part of the stored food will be divided up amongst her most appreciated workers
    else if (isHandoutDue)
    {
        int handoutAmount = storedFoodAmount * 0.5;
        storedFoodAmount -= handoutAmount;

        int maxWorkerScore = getCurrentMaxWorkerScore();

        for (int i = 0; i < WaspSlots::getMaxIndex() && handoutAmount > 0; i++)
        {
            int w_Index = wIndexSortedByWorkerScore[i];
            Wasp& worker = _allWasps[w_Index];
            WorkerDossier& dossier = workerDossiers[w_Index];
            int foodToGive = handoutAmount * 0.1; // In order of most to least appreciated worker, give part of the amount that is still left

             // While there is still handoutAmount left, give at least 1, and don't give more than needed to fill maxHungerSaturation
            foodToGive = std::max(1, foodToGive);
            foodToGive = std::min(foodToGive, (int) worker.balancedGenes.maxHungerSaturation - worker.hungerSaturation);
            
            worker.hungerSaturation += foodToGive;
            handoutAmount -= foodToGive;
        }
    }
}

/**
* Allows a worker wasp (with the given w_Index) to attempt to gift the given amount of food
* to the queen. Whether she accepted it is returned in the InteractionResponse.
*/
Queen::InteractionResponse Queen::receiveFood(int amount, int w_Index)
{
    if (!isAlive) { throw std::runtime_error("Wasp " + std::to_string(w_Index) + " tried to give food to a dead queen."); } 
    if (interactionsLeft <= 0) { return InteractionResponse::Denied; }

    hungerSaturation += amount;
    updateWorkerScore(w_Index, amount);


    if (hungerSaturation > balancedGenes.maxHungerSaturation) 
    { 
        storedFoodAmount += hungerSaturation - balancedGenes.maxHungerSaturation;
        hungerSaturation = balancedGenes.maxHungerSaturation;
    }
    return InteractionResponse::Accepted;
}

/**
* Returns the amount of food that the queen has currently stored beyond her own hunger saturation.
*/
int Queen::getStoredFoodAmount()
{
    return storedFoodAmount;
}

/**
* Returns the score that the queen has assigned to the worker wasp defined by the given w_Index.
*/
int Queen::getWorkerScore(int w_Index)
{
    return workerDossiers[w_Index].score;
}

/**
* Returns the highest current worker score of all the workers that the queen knows about. 
*/
int Queen::getCurrentMaxWorkerScore()
{
    int max = 0.0;
    for (int i = 0; i < WaspSlots::getMaxIndex(); i++)
    {
        WorkerDossier& dossier = workerDossiers[i];
        if (dossier.score > max) { max = dossier.score; }
    }
    return max;
}

/**
* Resets the queens knowledge of the worker wasp defined by the given w_Index. Useful for respawning wasps.
*/
void Queen::resetWorkerDossier(int w_Index)
{
    workerDossiers[w_Index] = WorkerDossier{w_Index, 0};
}


/**
* Updates 'wIndexSortedByWorkerScore' with the current worker score iteration.
* Due to performance reason, this is only done rarely when needed (like for handouts)
* and not at every iteration. Most times its better to keep everything sorted by wIndex.
*/
void Queen::sortIndicesByWorkerScore()
{
    std::sort(wIndexSortedByWorkerScore.begin(), wIndexSortedByWorkerScore.end(),
        [&](int a, int b) {
            return workerDossiers[a].score > workerDossiers[b].score;
        });
}

/**
* Updates the score of the worker wasp defined by the given w_Index.
*/
void Queen::updateWorkerScore(int w_Index, int scoreChange)
{
    WorkerDossier* dossier = &workerDossiers[w_Index];
    dossier->score += scoreChange;

    // Only the value retrieved through getWorkerScore is valid.
    // This wasp owned attribute is only used for fast debug rendering.
    _allWasps[w_Index]._debugWorkerScore = dossier->score;
}