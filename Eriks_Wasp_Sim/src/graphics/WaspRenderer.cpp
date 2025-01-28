#include "WaspRenderer.h"
#include <iostream>
#include "SimVisualizer.h"
#include "ModelHandler.h"
#include "ShaderHandler.h"
#include "ThreadPool.h"
#include "UI.h"
#include <thread>
#include <mutex>
#include "glm/ext.hpp"

//MESH
GLuint wasp_VAO;
GLuint wasp_VBO;
GLuint wasp_EBO;
GLuint wasp_instanceVBO;
int wasp_vertexCount;
const std::string modelFile = "wasp/Wasp.obj";

//SHADER
GLuint waspShaderProgram;
const std::string waspVertShaderFile = "wasp.vert";
const std::string waspFragShaderFile = "wasp.frag";

GLuint selectedWaspShaderProgram;
const std::string selectedWaspFragShaderFile = "selected_wasp.frag";

//THREADED INSTANCE DATA
std::vector<glm::mat4> wasp_instanceData;
std::mutex wasp_instanceDataMutex;
static const int threadPoolSize = 4;
static ThreadPool pool(threadPoolSize);

/**
* Initializes the WaspRenderer.
*/
void WaspRenderer::init()
{
    if (!ModelHandler::loadModel(modelFile, &wasp_VAO, &wasp_VBO, &wasp_EBO, &wasp_vertexCount))
    {
        std::cerr << "Failed to load wasp model" << std::endl;
        exit(EXIT_FAILURE);
    }
    ModelHandler::enableInstancing(&wasp_VAO, &wasp_instanceVBO);

    waspShaderProgram = ShaderHandler::buildShaderProgram(waspVertShaderFile, waspFragShaderFile);
    selectedWaspShaderProgram = ShaderHandler::buildShaderProgram(waspVertShaderFile, selectedWaspFragShaderFile);
}

/**
* Renders the given wasps. Assumes glut, glew, etc. are preinitialized.
*/
void WaspRenderer::drawWasps(std::vector<Wasp>* wasps)
{
    // COLLECT INSTANCE DATA
    wasp_instanceData.clear();

    // Divide wasp array into local sections and use threads to collect their data
    int maxIndex = WaspSlots::getMaxIndex();
    int sectionSize = std::floor(maxIndex / threadPoolSize);
    std::vector<std::thread> threads;
    for (int i = 0; i < threadPoolSize; ++i) 
    {
        int start = sectionSize * i;
        int end = i < threadPoolSize - 1 ? sectionSize * (i + 1) : maxIndex;
        pool.enqueue([start, end, &wasps] {_collectInstanceDataThreaded(wasps, start, end);});
    }
    pool.waitFinishAll();


    // DRAW
    ShaderHandler::drawInstanceData(&wasp_instanceData, &wasp_VAO, &wasp_instanceVBO, wasp_vertexCount, &waspShaderProgram);
}

/**
* Collects the necessary data for hardware instancing and safely inserts it into wasp_instanceData using a mutex. 
* A single thread works within a section of the wasps vector defined by the given start and end indices. 
* This ensures that each thread works on data with high memory/cache locality.
*/
void WaspRenderer::_collectInstanceDataThreaded(std::vector<Wasp>* wasps, int start, int end)
{
    std::vector<glm::mat4> localInstanceData;
    for (int i = start; i < end; ++i)
    {
        Wasp* wasp = &(*wasps)[i];
        if (!wasp->isAlive) { continue; }

        // Optimization: Instead of using atan2(), cos() and sin() which cause too much overhead
        // IMPORTANT: This only works based on the assumption that the components x and z are always normalized
        // The y component is allowed to change (which means the 3d vector does not need to be normalized)
        float cos = wasp->viewingVector.z;
        float sin = wasp->viewingVector.x;

        // Optimization: Manually construct model matrix instead of using transform and rotate functions
        // to skip any unnecessary steps.
        glm::mat4 model(1.0f);

        // Translate
        model[3][0] = wasp->position.x;
        model[3][1] = wasp->position.y;
        model[3][2] = wasp->position.z;

        // Rotate around Y axis
        model[0][0] = cos;
        model[0][2] = -sin;
        model[1][1] = 1.0f;
        model[2][0] = sin;
        model[2][2] = cos;

        localInstanceData.push_back(model);
    }

    // Merge local into global data
    std::lock_guard<std::mutex> lock(wasp_instanceDataMutex);
    wasp_instanceData.insert(wasp_instanceData.end(), localInstanceData.begin(), localInstanceData.end());
}

/**
* Visualizes the wasp selected by the user, provided it exists.
*/
void WaspRenderer::drawSelectedWasp() 
{
    //GET SELECTED WASP
    UI::UI_STATE* uiState = UI::getUIState();
    Wasp* wasp = uiState->selectedWasp;
    if (wasp == NULL)
    {
        return;
    }
    glm::vec3 position = wasp->position;

    // BUILD SINGLE INSTANCE DATA
    glm::mat4 model(1.0f);
    model = glm::translate(model, position);
    float angle = atan2(wasp->viewingVector.x, wasp->viewingVector.z);
    model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
    std::vector<glm::mat4> singleInstanceData(1, model);

    // DRAW WIREFRAME WITH DEPTH TESTING DISABLED
    glDisable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    ShaderHandler::drawInstanceData(&singleInstanceData, &wasp_VAO, &wasp_instanceVBO, wasp_vertexCount, &selectedWaspShaderProgram);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST);

    //DRAW GOAL
    if (uiState->drawSelectedWaspGoal && wasp->currentGoal != nullptr)
    {
        glm::vec3 goal = *wasp->currentGoal;

        glColor3f(0.0f, 1.0f, 1.0f);
        glBegin(GL_LINES);
        glVertex3f(position.x, position.y, position.z);
        glVertex3f(goal.x, goal.y, goal.z);
        glEnd();
    }
}