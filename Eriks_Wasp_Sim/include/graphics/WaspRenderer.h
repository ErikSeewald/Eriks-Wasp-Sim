#pragma once

#include "Wasp.h"
#include "WaspSlots.h"
#include <list>

 /**
 * @namespace WaspRenderer
 * @brief A namespace for all methods for rendering Wasps with OpenGL
 */
namespace WaspRenderer
{
    /**
    * Renders the given wasps. Assumes glut, glew, etc. are preinitialized.
    */
    void drawWasps(const std::vector<Wasp>& wasps);

    /**
    * Visualizes the wasp selected by the user, provided it exists.
    */
    void drawSelectedWasp();

    /**
    * Initializes the WaspRenderer. Loads models and builds shaders.
    */
    void init();
}