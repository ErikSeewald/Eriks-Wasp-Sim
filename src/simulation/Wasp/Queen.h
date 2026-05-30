#pragma once

#include "Wasp.h"
#include <vector>
#include <chrono>

/**
* @class Queen
* @brief The queen of the colony.
*/
class Queen : public Wasp
{
	public:
		/**
		* @struct WorkerDossier
		* @brief Knowledge that the queen has of a single worker wasp.
		*/
		struct WorkerDossier
		{
			int w_Index;
			int score;
		};

		/**
		* @enum InteractionResponse
		* @brief Different responses the queen can have to a wasp trying to interact with it.
		*/
		enum InteractionResponse
		{
			Accepted, Denied
		};

		static const int W_INDEX = -1;
		static constexpr float SCALE_FACTOR = 5.0f;
		static constexpr float INTERACTION_RADIUS = 10.0f;

		Queen();

		/**
		* Does queen-specific updating and then calls Wasp::update()
		*/
		void update(const std::chrono::duration<double>& deltaTime);

		/**
		* Allows a worker wasp (with the given w_Index) to attempt to gift the given amount of food
		* to the queen. Whether she accepted it is returned in the InteractionResponse.
		*/
		InteractionResponse receiveFood(int amount, int w_Index);

		/**
		* Returns the amount of food that the queen has currently stored beyond her own hunger saturation.
		*/
		int getStoredFoodAmount();

		/**
		* Returns the score that the queen has assigned to the worker wasp defined by the given w_Index.
		*/
		int getWorkerScore(int w_Index);

		/**
		* Returns the highest current worker score of all the workers that the queen knows about. 
		*/
		int getCurrentMaxWorkerScore();

		/**
		* Resets the queens knowledge of the worker wasp defined by the given w_Index. Useful for respawning wasps.
		*/
		void resetWorkerDossier(int w_Index);


	private:
		std::vector<Wasp>& _allWasps; // This vec does not represent the wasps that the queen actually KNOWS

		const int MAX_INTERACTIONS_PER_ITERATION = 4; // Don't want to overwork her
		int interactionsLeft; // How many interactions does the queen still allow at this iteration

		// During handouts, the queen can give things to her workers.
		const double SECONDS_BETWEEN_HANDOUTS = 5.0;
        double secondsSinceLastHandout = 0.0;
		bool isHandoutDue = false;

		std::vector<WorkerDossier> workerDossiers{};
		std::vector<int> wIndexSortedByWorkerScore; // Not updated every interation.

		int storedFoodAmount; // Additional food that the queen does not need for its own hunger saturation.

		/**
		* Updates 'wIndexSortedByWorkerScore' with the current worker score iteration.
		* Due to performance reason, this is only done rarely when needed (like for handouts)
		* and not at every iteration. Most times its better to keep everything sorted by wIndex.
		*/
		void sortIndicesByWorkerScore();

		/**
		* Updates the score of the worker wasp defined by the given w_Index.
		*/
		void updateWorkerScore(int w_Index, int scoreChange);

		/**
		* Manages how the queen's stored food should be used in the current update cycle.
		* Either for her own benefit or for that of her most appreciated workers.
		*/
		void updateFoodStorage();
};
