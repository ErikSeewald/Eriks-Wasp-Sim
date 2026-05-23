#include "WaspRenderer.h"
#include "WaspSlots.h"
#include <iostream>
#include "SimVisualizer.h"
#include "ModelHandler.h"
#include "ShaderHandler.h"
#include "InstancedRendering.h"
#include "DebugRenderer.h"
#include "ThreadPool.h"
#include "UI.h"
#include "RenderMode.h"
#include <thread>
#include <mutex>
#include "glm/ext.hpp"
#include <cstdint>

using InstancedRendering::InstanceDataWasp;

//MESH
GLuint wasp_VAO;
GLuint wasp_VBO;
GLuint wasp_EBO;
GLuint wasp_instanceVBO;
int wasp_vertexCount;
const std::string waspModelFile = "wasp/Wasp.obj";

GLuint queen_VAO;
GLuint queen_VBO;
GLuint queen_EBO;
GLuint queen_instanceVBO;
int queen_vertexCount;
const std::string queenModelFile = "wasp/Queen.obj";

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
    if (!ModelHandler::loadModel(waspModelFile, &wasp_VAO, &wasp_VBO, &wasp_EBO, &wasp_vertexCount))
    {
        std::cerr << "Failed to load wasp model" << std::endl;
        exit(EXIT_FAILURE);
    }
    InstancedRendering::setupInstancing<InstanceDataWasp>(wasp_VAO, &wasp_instanceVBO);

    if (!ModelHandler::loadModel(queenModelFile, &queen_VAO, &queen_VBO, &queen_EBO, &queen_vertexCount))
    {
        std::cerr << "Failed to load queen model" << std::endl;
        exit(EXIT_FAILURE);
    }
    InstancedRendering::setupInstancing<InstanceDataWasp>(queen_VAO, &queen_instanceVBO);

    waspShaderProgram = ShaderHandler::buildShaderProgram(waspVertShaderFile, waspFragShaderFile);
    selectedWaspShaderProgram = ShaderHandler::buildShaderProgram(waspVertShaderFile, selectedWaspFragShaderFile);
}

/**
* Constructs the base information in the bitmap that is used by the wasp shader.
* This bitmap is shared by many wasp entities, _modifyWaspBitmap being used to add entity-specific information.
*
* Format:
* 31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
* RM RM RM RM 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  G  Q
*
* With:
* RM - Byte representing RenderMode::WaspRenderMode
* G  - Does this wasp currently have a goal? Then 1, otherwise 0
* Q  - Is this wasp the queen? Then 1, otherwise 0
*/
inline uint32_t _constructWaspBitmap(bool isQueen)
{
    uint32_t bitmap = isQueen ? 0b1 : 0b0; // Q
    bitmap |= ((uint32_t) UI::getUIState()->waspRenderMode) << 28; // RM
    return bitmap;
}

/**
* Adds entity-specific information to the given baseBitmap.
* See _constructWaspBitmap
*/
inline uint32_t _modifyWaspBitmap(const uint32_t& baseBitmap, const Wasp& wasp)
{
    uint32_t bitmap = baseBitmap;
    bitmap |= wasp.currentGoal != nullptr ? 0b10 : 0b00; // G
    return bitmap;
}

/**
* Renders the given wasps.
*/
void WaspRenderer::drawWasps(const std::vector<Wasp>& wasps)
{
    const bool isQueen = false; // The queen is handled by its own function.
    uint32_t baseWaspBitmap = _constructWaspBitmap(isQueen); // Shared bitmap values for all normal wasps

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
                int idx = instanceIndex.fetch_add(1, std::memory_order_relaxed);
                wasp_instanceData[idx] = InstanceDataWasp{ w.position, w.viewingVector, i, _modifyWaspBitmap(baseWaspBitmap, w)};
            }
        });
    }

    pool.waitFinishAll();

    // Shrink to size of added instances
    wasp_instanceData.resize(instanceIndex.load(std::memory_order_relaxed));

    InstancedRendering::drawInstanceData(wasp_instanceData, wasp_VAO, wasp_instanceVBO, wasp_vertexCount, waspShaderProgram);
}

/**
* Draws the given queen.
*/
void WaspRenderer::drawQueen(const Queen& queen)
{
    if (!queen.isAlive) { return; }

    const bool isQueen = true;
    uint32_t queenBitmap = _constructWaspBitmap(isQueen);
    std::vector<InstanceDataWasp> singleInstanceData(1, InstanceDataWasp{ queen.position, queen.viewingVector, Queen::W_INDEX, queenBitmap });
    InstancedRendering::drawInstanceData(singleInstanceData, queen_VAO, queen_instanceVBO, queen_vertexCount, waspShaderProgram);
}

/**
* Visualizes the wasp selected by the user, provided it exists.
*/
void WaspRenderer::drawSelectedWasp() 
{
    UI::UI_STATE* uiState = UI::getUIState();
    Wasp* wasp = uiState->selectedWasp;
    if (wasp == nullptr)
    {
        return;
    }
    std::vector<InstanceDataWasp> singleInstanceData(1, InstanceDataWasp{ wasp->position, wasp->viewingVector });

    // DRAW WIREFRAME WITH DEPTH TESTING DISABLED
    glDisable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    if (wasp == &WaspSlots::getQueen())
    { InstancedRendering::drawInstanceData(singleInstanceData, queen_VAO, queen_instanceVBO, queen_vertexCount, selectedWaspShaderProgram); }
    
    else
    { InstancedRendering::drawInstanceData(singleInstanceData, wasp_VAO, wasp_instanceVBO, wasp_vertexCount, selectedWaspShaderProgram); }

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST);

    //DRAW GOAL
    if (uiState->drawSelectedWaspGoal && wasp->currentGoal != nullptr)
    {
        DebugRenderer::scheduleLine(wasp->position, *wasp->currentGoal, goalVecColor);
    }
}