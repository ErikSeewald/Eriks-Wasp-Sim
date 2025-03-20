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

	glm::mat4 projection;
	glm::mat4 view;

	float pitchRad = -0.53f;
	float yawRad = -2.35f;
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
	static const float FOV_RADIANS = glm::radians(FOV_DEGREES);
}