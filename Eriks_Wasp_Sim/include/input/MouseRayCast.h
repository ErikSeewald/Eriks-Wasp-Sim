#pragma once

#include "Wasp.h"
#include <glm/glm.hpp>

/**
* @namespace MouseRayCast
* @brief A namespace for all methods to handle mouse click ray casting.
*/
namespace MouseRayCast
{
    /**
    * Selects the closest wasp in the scene at the given 2D screen coordinates.
    * 
    * @param x the x screen coordinate
    * @param y the y screen coordinate
    * @return the selected wasp or NULL if no wasp was selected
    */
    Wasp* selectWasp(int x, int y);

    /**
    * Converts the given 2D screen coordinates into a ray going out from the screen into the scene where
    * the mouse was clicked.
    * 
    * @param x the x coordinate of the click
    * @param y the y coordinate of the click
    * @return the converted vec3 ray
    */
    glm::vec3 _castClickRay(int x, int y);
}