#pragma once

#include "Wasp.h"
#include "WaspSlots.h"
#include <list>

using EntitySlots::EntitySlot;

 /**
 * @namespace WaspRenderer
 * @brief A namespace for all methods for rendering Wasps with OpenGL
 */
namespace WaspRenderer
{
    /**
    * Visualizes the given WaspSlot linked list. Assumes glut, glew, etc. are preinitialized.
    */
    void drawWasps(EntitySlot* waspSlot);

    /**
    * Visualizes the wasp selected by the user, provided it exists.
    */
    void drawSelectedWasp();

    /**
    * Initializes the WaspRenderer.
    */
    void init();
}