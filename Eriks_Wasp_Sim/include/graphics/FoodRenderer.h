#pragma once

#include "food.h"

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
    * Draws the given food entity with the assumption that the correct vertex array has already been bound and the
    * color has been set.
    */
    void _drawFoodPrebound(FoodEntity* food);

    /**
    * Initializes the FoodRenderer.
    */
    void init();
}