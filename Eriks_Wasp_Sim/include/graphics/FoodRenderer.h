#pragma once

#include "Food.h"
#include <vector>
#include "SimVisualizer.h"

using Food::FoodEntity;

 /**
 * @namespace FoodRenderer
 * @brief A namespace for all methods for rendering Food with OpenGL
 */
namespace FoodRenderer
{
    /**
    * Renders the food entities. Assumes glut, glew, etc. are preinitialized.
    */
    void drawFood(const std::vector<FoodEntity>& foodEntities);

    /**
    * Initializes the FoodRenderer. Loads models and builds shaders.
    */
    void init();
}
