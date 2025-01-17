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
    void drawWasps(std::vector<Wasp>* wasps);

    /**
    * Collects the necessary data for hardware instancing and safely inserts it into wasp_instanceData using a mutex.
    * A single thread works within a section of the wasps vector defined by the given start and end indices.
    * This ensures that each thread works on data with high memory/cache locality.
    */
    void _collectInstanceDataThreaded(std::vector<Wasp>* wasps, int start, int end);

    /**
    * Visualizes the wasp selected by the user, provided it exists.
    */
    void drawSelectedWasp();

    /**
    * Initializes the WaspRenderer.
    */
    void init();
}