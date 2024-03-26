#pragma once

#include "food.h"

using EntitySlots::EntitySlot;

 /**
 * @namespace FoodRenderer
 * @brief A namespace for all methods for rendering Food with OpenGL
 */
namespace FoodRenderer
{
    /**
    * Visualizes the given FoodSlot linked list. Assumes glut, glew, etc. are preinitialized.
    */
    void drawFoodSlots(EntitySlot* foodSlot);

    /**
    * Initializes the FoodpRenderer.
    */
    void init();
}