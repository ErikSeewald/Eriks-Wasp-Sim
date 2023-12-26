/**
 * @file wasp.h
 * @brief This file serves as the header for wasp.cpp.
 */

#include <DirectXMath.h>

using DirectX::XMFLOAT3;

/**
* @class Wasp
* @brief A single wasp within the simulation.
*/
class Wasp 
{
	public:
		XMFLOAT3 getPosition();
		void setPosition(XMFLOAT3 position);

	private:
		XMFLOAT3 position;
};