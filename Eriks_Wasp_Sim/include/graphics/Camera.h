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