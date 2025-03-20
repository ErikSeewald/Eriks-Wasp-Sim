#include "FoodRenderer.h"
#include "SimVisualizer.h"
#include "ModelHandler.h"
#include "ShaderHandler.h"
#include "InstancedRendering.h"
#include <vector>
#include <iostream>
#include <thread>
#include <mutex>

using Food::FoodEntity;
using InstancedRendering::InstanceDataBasic;

//MESH
GLuint food_VAO;
GLuint food_VBO;
GLuint food_EBO;
GLuint food_instanceVBO;
int food_vertexCount;
const std::string modelFile = "food/Food.obj";
const glm::vec4 foodColor = glm::vec4(0.4f, 0.1f, 0.05f, 1.0f);

//SHADER
GLuint foodShaderProgram;
const std::string foodVertShaderFile = "food.vert";
const std::string foodFragShaderFile = "food.frag";

//THREADED INSTANCE DATA
std::vector<InstanceDataBasic> food_instanceData;
std::mutex food_instanceDataMutex;

/**
* Initializes the FoodRenderer. Loads models and builds shaders.
*/
void FoodRenderer::init()
{
    if (!ModelHandler::loadModel(modelFile, &food_VAO, &food_VBO, &food_EBO, &food_vertexCount))
    {
        std::cerr << "Failed to load food model" << std::endl;
        exit(EXIT_FAILURE);
    }
    InstancedRendering::setupInstancing<InstanceDataBasic>(food_VAO, &food_instanceVBO);

    foodShaderProgram = ShaderHandler::buildShaderProgram(foodVertShaderFile, foodFragShaderFile);
}

/**
* Renders the food entities. Assumes glut, glew, etc. are preinitialized.
*/
void FoodRenderer::drawFood(const std::vector<FoodEntity>& foodEntities)
{
    // COLLECT INSTANCE DATA
    food_instanceData.clear();

    // Collect instance data
    // TODO: Currently, with a SLOT_COUNT of only 10000 this is faster than threading. Maybe change this later.
    int maxIndex = Food::getMaxIndex();
    for (int i = 0; i < maxIndex; ++i)
    {
        const FoodEntity& food = foodEntities[i];
        if (food.eaten) { continue; }
        food_instanceData.push_back(InstanceDataBasic{ food.position, foodColor });
    }

    // DRAW
    InstancedRendering::drawInstanceData(food_instanceData, food_VAO, food_instanceVBO, food_vertexCount, foodShaderProgram);
}