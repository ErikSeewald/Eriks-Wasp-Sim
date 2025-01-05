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
    * Collects the necessary instance data for hardware instancing for all EntitySlots matching the given
    * offset and step size to allow for multithreaded traversal. Calling this function with 5 threads
    * would mean splitting the workload evenly among 5 function calls, each thread i has t_offset = i and t_step = 5.
    */
    void _collectInstanceDataThreaded(EntitySlot* startSlot, int t_offset, int t_step);

    /**
    * Visualizes the wasp selected by the user, provided it exists.
    */
    void drawSelectedWasp();

    /**
    * Initializes the WaspRenderer.
    */
    void init();
}