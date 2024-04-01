#include "FoodRenderer.h"
#include "SimVisualizer.h"
#include "ModelHandler.h"
#include <vector>
#include <iostream>

using Food::FoodEntity;

//MESH
GLuint food_VAO;
int food_vertexCount;

//FILE
const std::string baseDir = "../../../../../Assets/Models/food/";
const std::string baseDirFallback = "../../../Assets/Models/food/";
const std::string modelFile = baseDir + "Food.obj";
const std::string modelFileFallback = baseDirFallback + "Food.obj";

/**
* Initializes the FoodRenderer.
*/
void FoodRenderer::init()
{
    if (!ModelHandler::loadModel(baseDir, modelFile, &food_VAO, &food_vertexCount))
    {
        // FALLBACK
        if (!ModelHandler::loadModel(baseDirFallback, modelFileFallback, &food_VAO, &food_vertexCount))
        {
            std::cerr << "Failed to load food model" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
}

/**
* Visualizes the given FoodSlot linked list. Assumes glut, glew, etc. are preinitialized.
*/
void FoodRenderer::drawFood(EntitySlot* foodSlot)
{
    glColor3f(0.4f, 0.1f, 0.05f);
    glBindVertexArray(food_VAO);
    while (foodSlot != nullptr)
    {
        _drawFoodPrebound((FoodEntity*) foodSlot->entity);
        foodSlot = foodSlot->next;
    }
    glBindVertexArray(0);
}

/**
* Draws the given food entity with the assumption that the correct vertex array has already been bound and the
* color has been set.
*/
void FoodRenderer::_drawFoodPrebound(FoodEntity* food)
{
    glPushMatrix();
    glTranslatef(food->position.x, food->position.y, food->position.z);

    glDrawElements(GL_TRIANGLES, food_vertexCount, GL_UNSIGNED_INT, 0);

    glPopMatrix();
}