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
    * Initializes the FoodRenderer.
    */
    void init();
}