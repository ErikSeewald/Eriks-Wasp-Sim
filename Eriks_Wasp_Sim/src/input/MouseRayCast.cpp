#include "MouseRayCast.h"

#include "MouseHandler.h"
#include <glm/gtc/matrix_inverse.hpp>
#include <limits>
#include "Simulation.h"
#include "SimVisualizer.h"

/**
* Selects the closest wasp in the scene at the given 2D screen coordinates.
*
* @param x the x screen coordinate
* @param y the y screen coordinate
* @return the selected wasp or NULL if no wasp was selected
*/
Wasp* MouseRayCast::selectWasp(int x, int y) 
{
    glm::vec3 clickRay = _castClickRay(x, y);
    glm::vec3 rayOrigin = SimVisualizer::getCamera().position;
    glm::vec3 camDirection = SimVisualizer::getCamera().direction;

    Wasp* selectedWasp{};
    double shortestDistance = std::numeric_limits<double>::infinity();

    for (Wasp* wasp : *Simulation::getWasps())
    {
        glm::vec3 camToWasp = rayOrigin - wasp->getPosition();
        if (glm::dot(camToWasp, camDirection) > 0)
        {
            // Do not check wasps that are behind the camera
            continue;
        }
        
        // If the cross product between camToWasp and the clickRay is small enough, then camToWasp is
        // close enough to the clickRay that we count it as hitting the wasp
        glm::vec3 crossProduct = glm::cross(camToWasp, clickRay);
        if (glm::length(crossProduct) < 0.5)
        {
            double newDistance = glm::length(camToWasp);
            if (newDistance < shortestDistance)
            {
                shortestDistance = newDistance;
                selectedWasp = wasp;
            }
        }
    }

    return selectedWasp;
}

/**
* Converts the given 2D screen coordinates into a ray going out from the screen into the scene where
* the mouse was clicked.
*
* @param x the x coordinate of the click
* @param y the y coordinate of the click
* @return the converted vec3 ray
*/
#include <iostream>
glm::vec3 MouseRayCast::_castClickRay(int x, int y)
{
    //WINDOW SIZE
    int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
    int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

    // CONVERT TO NORMALIZED DEVICE COORDINATES
    glm::vec2 ndc{};
    ndc.x = (2.0f * x) / windowWidth - 1.0f; // convert to range [-1, 1]
    ndc.y = 1.0f - (2.0f * y) / windowHeight; // convert to range [1, -1] (flipped because screen origin at top left)
    glm::vec4 rayClip = glm::vec4(ndc.x, ndc.y, -1.0f, 1.0f);

    //GET PROJECTION MATRIX
    glm::mat4 projectionMatrix{};
    glGetFloatv(GL_PROJECTION_MATRIX, &projectionMatrix[0][0]);

    //RAY TO EYE COORDINATES
    glm::vec4 rayEye = glm::inverse(projectionMatrix) * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0, 0.0);

    //GET VIEW MATRIX
    glm::mat4 viewMatrix{};
    glGetFloatv(GL_MODELVIEW_MATRIX, &viewMatrix[0][0]);

    //RAY TO WORLD COORDINATES
    return glm::normalize(glm::inverse(viewMatrix) * rayEye);
}