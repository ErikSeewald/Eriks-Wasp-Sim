#include "WaspRenderer.h"
#include <iostream>
#include "SimVisualizer.h"
#include "ModelHandler.h"
#include "ShaderHandler.h"
#include "InstancedRendering.h"
#include "DebugRenderer.h"
#include "ThreadPool.h"
#include "UI.h"
#include <thread>
#include <mutex>
#include "glm/ext.hpp"

using InstancedRendering::InstanceDataWasp;

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
const std::string selectedWaspFragShaderFile = "selected_entity.frag";
const glm::vec4 goalVecColor = glm::vec4(0.2f, 0.5f, 1.0f, 1.0f);

//THREADED INSTANCE DATA
std::vector<InstanceDataWasp> wasp_instanceData;
std::mutex wasp_instanceDataMutex;
static const int threadPoolSize = ThreadPool::choosePoolSize();
static ThreadPool pool(threadPoolSize);

/**
* Initializes the WaspRenderer. Loads models and builds shaders.
*/
void WaspRenderer::init()
{
    if (!ModelHandler::loadModel(modelFile, &wasp_VAO, &wasp_VBO, &wasp_EBO, &wasp_vertexCount))
    {
        std::cerr << "Failed to load wasp model" << std::endl;
        exit(EXIT_FAILURE);
    }
    InstancedRendering::setupInstancing<InstanceDataWasp>(wasp_VAO, &wasp_instanceVBO);

    waspShaderProgram = ShaderHandler::buildShaderProgram(waspVertShaderFile, waspFragShaderFile);
    selectedWaspShaderProgram = ShaderHandler::buildShaderProgram(waspVertShaderFile, selectedWaspFragShaderFile);
}

/**
* Renders the given wasps. Assumes glut, glew, etc. are preinitialized.
*/
void WaspRenderer::drawWasps(const std::vector<Wasp>& wasps)
{
    std::atomic<size_t> instanceIndex(0); // Thread safe index into wasp_instanceData

    int maxIndex = WaspSlots::getMaxIndex();
    wasp_instanceData.resize(maxIndex);

    // Note: Updating wasp_instanceData like this every frame ends up being better than having it updated by
    // the simulation threads whenever it changes and letting the shader check isAlive. 
    // It only frees up some time on devices with dedicated GPUs, but not enough to be worth it. 
    // On devices with integrated GPUs that approach is actually slower.

    // Use threads to collect the necessary data for hardware instancing and safely insert it into wasp_instanceData. 
    // A single thread works on section of the wasps vector defined by the start and end indices.
    // This leads to high memory/cache locality.
    int sectionSize = std::floor(maxIndex / threadPoolSize);
    for (int t = 0; t < threadPoolSize; ++t)
    {
        int start = sectionSize * t;
        int end = t < threadPoolSize - 1 ? sectionSize * (t + 1) : maxIndex;

        pool.enqueue([&, start, end]() {
            for (int i = start; i < end; ++i)
            {
                const Wasp& w = wasps[i];
                if (!w.isAlive) { continue; }

                // weakest memory ordering that still guarantees atomicity
                size_t idx = instanceIndex.fetch_add(1, std::memory_order_relaxed);
                wasp_instanceData[idx] = InstanceDataWasp{ w.position, w.viewingVector };
            }
        });
    }

    pool.waitFinishAll();

    // Shrink to size of added instances
    wasp_instanceData.resize(instanceIndex.load(std::memory_order_relaxed));

    InstancedRendering::drawInstanceData(wasp_instanceData, wasp_VAO, wasp_instanceVBO, wasp_vertexCount, waspShaderProgram);
}

/**
* Visualizes the wasp selected by the user, provided it exists.
*/
void WaspRenderer::drawSelectedWasp() 
{
    //GET SELECTED WASP
    UI::UI_STATE* uiState = UI::getUIState();
    Wasp* wasp = uiState->selectedWasp;
    if (wasp == nullptr)
    {
        return;
    }

    // SINGLE INSTANCE DATA
    std::vector<InstanceDataWasp> singleInstanceData(1, InstanceDataWasp{ wasp->position, wasp->viewingVector });

    // DRAW WIREFRAME WITH DEPTH TESTING DISABLED
    glDisable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    InstancedRendering::drawInstanceData(singleInstanceData, wasp_VAO, wasp_instanceVBO, wasp_vertexCount, selectedWaspShaderProgram);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST);

    //DRAW GOAL
    if (uiState->drawSelectedWaspGoal && wasp->currentGoal != nullptr)
    {
        DebugRenderer::scheduleLine(wasp->position, *wasp->currentGoal, goalVecColor);
    }
}