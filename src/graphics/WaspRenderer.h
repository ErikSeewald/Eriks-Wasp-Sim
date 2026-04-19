#pragma once

#include "Wasp.h"
#include "Queen.h"
#include <list>

 /**
 * @namespace WaspRenderer
 * @brief A namespace for all methods for rendering Wasps with OpenGL
 */
namespace WaspRenderer
{
    /**
    * Renders the given wasps.
    */
    void drawWasps(const std::vector<Wasp>& wasps);

    /**
    * Draws the given queen.
    */
    void drawQueen(const Queen& queen);

    /**
    * Visualizes the wasp selected by the user, provided it exists.
    */
    void drawSelectedWasp();

    /**
    * Initializes the WaspRenderer. Loads models and builds shaders.
    */
    void init();
}