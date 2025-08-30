#include "RNG.h"
#include <random>

/**
* Returns a positive random float between lower and upper. Note that this function does not support negative bounds.
*/
float RNG::randBetween(float lower, float upper)
{
	if (lower > upper)
	{
		float temp = lower;
		lower = upper;
		upper = temp;
	}

	float difference = upper - lower;
	return ((float) std::rand() / RAND_MAX) * difference + lower;
}

int RNG::randMod(int modulo)
{
	return std::rand() % modulo;
}