/**
 * @file Camera.h
 * @brief This file contains the definition of the Camera struct.
 */

#pragma once

#include <DirectXMath.h>

/**
* @struct Camera
* @brief A struct holding position, direction, pitch and yaw of the glut camera.
*/
struct Camera 
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 direction;

	float pitch = -30.0f;
	float yaw = -130.0f;
};