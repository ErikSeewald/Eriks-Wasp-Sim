#include "Queen.h"
#include "WaspSlots.h"

Queen::Queen() : Wasp(W_INDEX)
{
    // All workers start with a score of 0
    for (int i = 0; i < WaspSlots::getSlotCount(); i++)
    {
        workerDossiers.emplace_back(WorkerDossier{i, 0});
    }
}; 

/**
* Does queen-specific updating and then calls Wasp::update()
*/
void Queen::update()
{
    interactionsLeft = MAX_INTERACTIONS_PER_ITERATION;
    Wasp::update();
}

/**
* Allows a worker wasp (with the given w_Index) to attempt to gift the given amount of food
* to the queen. Whether she accepted it is returned in the InteractionResponse.
*/
Queen::InteractionResponse Queen::receiveFood(int amount, int w_Index)
{
    if (interactionsLeft <= 0) { return InteractionResponse::Denied; }

    hungerSaturation += amount;
    updateWorkerScore(w_Index, amount);

    return InteractionResponse::Accepted;
}

/**
* Returns the score that the queen has assigned to the worker wasp defined by the given w_Index.
*/
int Queen::getWorkerScore(int w_Index)
{
    return workerDossiers[w_Index].score;
}

/**
* Resets the queens knowledge of the worker wasp defined by the given w_Index. Useful for respawning wasps.
*/
void Queen::resetWorkerDossier(int w_Index)
{
    workerDossiers[w_Index] = WorkerDossier{w_Index, 0};
}

/**
* Updates the score of the worker wasp defined by the given w_Index.
*/
void Queen::updateWorkerScore(int w_Index, int scoreChange)
{
    WorkerDossier* dossier = &workerDossiers[w_Index];
    dossier->score += scoreChange;
}