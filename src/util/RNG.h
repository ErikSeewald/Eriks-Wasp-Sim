
/**
* @namespace RNG
* @brief A namespace for all RNG related utility functions.
*/
namespace RNG
{
	/**
	* Returns a positive random float between lower and upper. Note that this function does not support negative bounds.
	*/
	float randBetween(float lower, float upper);

	int randMod(int modulo);
}