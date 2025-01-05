#include "WaspRenderer.h"
#include <iostream>
#include "SimVisualizer.h"
#include "ModelHandler.h"
#include "ShaderHandler.h"
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
* Visualizes the given WaspSlot linked list. Assumes glut, glew, etc. are preinitialized.
*/
void WaspRenderer::drawWasps(EntitySlot* waspSlot)
{
    // COLLECT INSTANCE DATA
    wasp_instanceData.clear();

    // Run threads
    static const int numThreads = 5;
    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; ++i) 
    {
        threads.emplace_back(_collectInstanceDataThreaded, waspSlot, i, numThreads);
    }

    for (std::thread& t : threads) {t.join();}

    // DRAW
    ShaderHandler::drawInstanceData(&wasp_instanceData, &wasp_VAO, &wasp_instanceVBO, wasp_vertexCount, &waspShaderProgram);
}

/**
* Collects the necessary instance data for hardware instancing for all EntitySlots matching the given
* offset and step size to allow for multithreaded traversal. Calling this function with 5 threads
* would mean splitting the workload evenly among 5 function calls, each thread i has t_offset = i and t_step = 5.
* The given selectedWasp pointer is used to avoid drawing the selected wasp.
*/
void WaspRenderer::_collectInstanceDataThreaded(EntitySlot* startSlot, int t_offset, int t_step)
{
    EntitySlot* currentSlot = startSlot;
    int index = 0;

    std::vector<glm::mat4> localInstanceData;
    while (currentSlot != nullptr)
    {
        if ((index % t_step) == t_offset)
        {
            Wasp* wasp = (Wasp*) currentSlot->entity;

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
        currentSlot = currentSlot->next;
        ++index;
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
    if (uiState->drawSelectedWaspGoal && wasp->getCurrentGoal() != nullptr)
    {
        glm::vec3 goal = *wasp->getCurrentGoal();

        glColor3f(0.0f, 1.0f, 1.0f);
        glBegin(GL_LINES);
        glVertex3f(position.x, position.y, position.z);
        glVertex3f(goal.x, goal.y, goal.z);
        glEnd();
    }
}