#include "FoodRenderer.h"
#include "SimVisualizer.h"
#include "ModelHandler.h"
#include "ShaderHandler.h"
#include <vector>
#include <iostream>
#include <thread>
#include <mutex>

using Food::FoodEntity;

//MESH
GLuint food_VAO;
GLuint food_VBO;
GLuint food_EBO;
GLuint food_instanceVBO;
int food_vertexCount;
const std::string modelFile = "food/Food.obj";

//SHADER
GLuint foodShaderProgram;
const std::string foodVertShaderFile = "food.vert";
const std::string foodFragShaderFile = "food.frag";

//THREADED INSTANCE DATA
std::vector<glm::mat4> food_instanceData;
std::mutex food_instanceDataMutex;

/**
* Initializes the FoodRenderer.
*/
void FoodRenderer::init()
{
    if (!ModelHandler::loadModel(modelFile, &food_VAO, &food_VBO, &food_EBO, &food_vertexCount))
    {
        std::cerr << "Failed to load food model" << std::endl;
        exit(EXIT_FAILURE);
    }
    ModelHandler::enableInstancing(&food_VAO, &food_instanceVBO);

    foodShaderProgram = ShaderHandler::buildShaderProgram(foodVertShaderFile, foodFragShaderFile);
}

/**
* Renders the food entities. Assumes glut, glew, etc. are preinitialized.
*/
void FoodRenderer::drawFood(std::vector<FoodEntity>* foodEntities)
{
    // COLLECT INSTANCE DATA
    food_instanceData.clear();

    // Collect instance data
    // TODO: Currently, with a SLOT_COUNT of only 1000 this is faster than threading. Maybe change this later.
    for (int i = 0; i < Food::SLOT_COUNT; ++i)
    {
        FoodEntity* food = &(*foodEntities)[i];
        if (food->eaten) { continue; }

        // Optimization: Manually construct model matrix instead of using transform and rotate functions
        // to skip any unnecessary steps.
        glm::mat4 model(1.0f);
        model[3][0] = food->position.x;
        model[3][1] = food->position.y;
        model[3][2] = food->position.z;

        food_instanceData.push_back(model);
    }

    // DRAW
    ShaderHandler::drawInstanceData(&food_instanceData, &food_VAO, &food_instanceVBO, food_vertexCount, &foodShaderProgram);
}