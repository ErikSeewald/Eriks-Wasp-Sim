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
* Visualizes the given FoodSlot linked list. Assumes glut, glew, etc. are preinitialized.
*/
void FoodRenderer::drawFood(EntitySlot* foodSlot)
{
    // COLLECT INSTANCE DATA
    food_instanceData.clear();

    // Run threads
    static const int numThreads = 4;
    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; ++i)
    {
        threads.emplace_back(_collectInstanceDataThreaded, foodSlot, i, numThreads);
    }

    for (std::thread& t : threads) { t.join(); }

    // DRAW
    ShaderHandler::drawInstanceData(&food_instanceData, &food_VAO, &food_instanceVBO, food_vertexCount, &foodShaderProgram);
}

/**
* Collects the necessary instance data for hardware instancing for all EntitySlots matching the given
* offset and step size to allow for multithreaded traversal. Calling this function with 5 threads
* would mean splitting the workload evenly among 5 function calls, each thread i has t_offset = i and t_step = 5.
*/
void FoodRenderer::_collectInstanceDataThreaded(EntitySlot* startSlot, int t_offset, int t_step)
{
    EntitySlot* currentSlot = startSlot;
    int index = 0;

    std::vector<glm::mat4> localInstanceData;
    while (currentSlot != nullptr)
    {
        if ((index % t_step) == t_offset)
        {
            FoodEntity* food = (FoodEntity*) currentSlot->entity;

            // Optimization: Manually construct model matrix instead of using transform and rotate functions
            // to skip any unnecessary steps.
            glm::mat4 model(1.0f);
            model[3][0] = food->position.x;
            model[3][1] = food->position.y;
            model[3][2] = food->position.z;

            localInstanceData.push_back(model);
            
        }
        currentSlot = currentSlot->next;
        ++index;
    }

    // Merge local into global data
    std::lock_guard<std::mutex> lock(food_instanceDataMutex);
    food_instanceData.insert(food_instanceData.end(), localInstanceData.begin(), localInstanceData.end());
}