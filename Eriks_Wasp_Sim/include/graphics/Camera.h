#pragma once

#include <glm/glm.hpp>

/**
* @struct Camera
* @brief A struct holding position, direction, pitch and yaw of the glut camera.
*/
struct Camera 
{
	glm::vec3 position;
	glm::vec3 direction;

	float pitch = -30.0f;
	float yaw = -130.0f;
};

/**
* @namespace CameraSettings
* @brief Namespace for important settings related to the camera used for rendering the simulation.
*/
namespace CameraSettings
{
	static const float NEAR_CLIP = 0.1f;
	static const float FAR_CLIP = 500.0f;
	static const float FOV_DEGREES = 60.0f;
}