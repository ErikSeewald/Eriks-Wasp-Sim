#pragma once

#include "food.h"
#include <vector>
#include "SimVisualizer.h"

using EntitySlots::EntitySlot;
using Food::FoodEntity;

 /**
 * @namespace FoodRenderer
 * @brief A namespace for all methods for rendering Food with OpenGL
 */
namespace FoodRenderer
{
    /**
    * Visualizes the given FoodSlot linked list. Assumes glut, glew, etc. are preinitialized.
    */
    void drawFood(EntitySlot* foodSlot);

    /**
    * Collects the necessary instance data for hardware instancing for all EntitySlots matching the given
    * offset and step size to allow for multithreaded traversal. Calling this function with 5 threads
    * would mean splitting the workload evenly among 5 function calls, each thread i has t_offset = i and t_step = 5.
    */
    void _collectInstanceDataThreaded(EntitySlot* startSlot, int t_offset, int t_step);

    /**
    * Initializes the FoodRenderer.
    */
    void init();
}