#pragma once

#include "Wasp.h"

/**
* @class Queen
* @brief The queen of the colony.
*/
class Queen : public Wasp
{
	public:
		static const int W_INDEX = -1;
		static constexpr float SCALE_FACTOR = 5.0f;

		Queen() : Wasp(W_INDEX) {};
};
