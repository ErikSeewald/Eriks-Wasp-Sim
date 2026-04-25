#pragma once

#include "Wasp.h"

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
		void update();

		/**
		* Allows a worker wasp (with the given w_Index) to attempt to gift the given amount of food
		* to the queen. Whether she accepted it is returned in the InteractionResponse.
		*/
		InteractionResponse receiveFood(int amount, int w_Index);

		/**
		* Returns the score that the queen has assigned to the worker wasp defined by the given w_Index.
		*/
		int getWorkerScore(int w_Index);

		/**
		* Resets the queens knowledge of the worker wasp defined by the given w_Index. Useful for respawning wasps.
		*/
		void resetWorkerDossier(int w_Index);

	private:
		const int MAX_INTERACTIONS_PER_ITERATION = 4; // Don't want to overwork her
		int interactionsLeft; // How many interactions does the queen still allow at this iteration

		std::vector<WorkerDossier> workerDossiers{};

		/**
		* Updates the score of the worker wasp defined by the given w_Index.
		*/
		void updateWorkerScore(int w_Index, int scoreChange);
};
