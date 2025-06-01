#include "DebugRenderer.h"
#include "SimVisualizer.h"
#include "ModelHandler.h"
#include "ShaderHandler.h"
#include "InstancedRendering.h"
#include <iostream>

using InstancedRendering::InstanceDataBasic;
using InstancedRendering::InstanceDataLine;

const glm::vec3 rootVec = glm::vec3(0, 0, 0);
const glm::vec3 axisXVec = glm::vec3(10, 0, 0);
const glm::vec3 axisYVec = glm::vec3(0, 10, 0);
const glm::vec3 axisZVec = glm::vec3(0, 0, 10);

const glm::vec4 axisXColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
const glm::vec4 axisYColor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
const glm::vec4 axisZColor = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

//MESH
GLuint grid_VAO;
GLuint grid_VBO;
GLuint grid_EBO;
GLuint grid_instanceVBO;
int grid_vertexCount;
const std::string gridModelFile = "debug/Grid.obj";

GLuint line_VAO;
GLuint line_VBO;
GLuint line_EBO;
GLuint line_instanceVBO;
int line_vertexCount;
const std::string lineModelFile = "debug/Line.obj";

//SHADER
GLuint lineShaderProgram;
const std::string lineVertShaderFile = "line.vert";
const std::string colorFragShaderFile = "instance_color.frag";

GLuint gridShaderProgram;
const std::string basicVertShaderFile = "instance_basic.vert";

//INSTANCES
std::vector<InstanceDataBasic> gridInstanceData{ InstanceDataBasic{ rootVec, glm::vec4(0.3f, 0.3f, 0.3f, 1.0f)}};
std::vector<InstanceDataLine> linesInstanceData{};

/**
* Initializes the DebugRenderer. Loads models and builds shaders.
*/
void DebugRenderer::init()
{
    // GRID
    if (!ModelHandler::loadModel(gridModelFile, &grid_VAO, &grid_VBO, &grid_EBO, &grid_vertexCount))
    {
        std::cerr << "Failed to load grid model" << std::endl;
        exit(EXIT_FAILURE);
    }
    InstancedRendering::setupInstancing<InstanceDataBasic>(grid_VAO, &grid_instanceVBO);

    // LINE
    if (!ModelHandler::loadModel(lineModelFile, &line_VAO, &line_VBO, &line_EBO, &line_vertexCount))
    {
        std::cerr << "Failed to load line model" << std::endl;
        exit(EXIT_FAILURE);
    }
    InstancedRendering::setupInstancing<InstanceDataLine>(line_VAO, &line_instanceVBO);

    // SHADERS
    gridShaderProgram = ShaderHandler::buildShaderProgram(basicVertShaderFile, colorFragShaderFile);
    lineShaderProgram = ShaderHandler::buildShaderProgram(lineVertShaderFile, colorFragShaderFile);
}

/**
* Draws a reference unit grid at the center of the coordinate system.
*/
void DebugRenderer::drawGrid()
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    InstancedRendering::drawInstanceData<InstanceDataBasic>(gridInstanceData, grid_VAO, grid_instanceVBO, grid_vertexCount, gridShaderProgram);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    scheduleLine(rootVec, axisXVec, axisXColor);
    scheduleLine(rootVec, axisYVec, axisYColor);
    scheduleLine(rootVec, axisZVec, axisZColor);
}

/**
* Schedules a line to be drawn between the given start and end coordinates with the given color.
* DebugRenderer::drawScheduledLines draws all previously scheduled lines at once
* using instanced rendering.
*/
void DebugRenderer::scheduleLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color)
{
    linesInstanceData.push_back(InstanceDataLine{start, end, color});
}

/**
* Draws previously scheduled lines (DebugRenderer::scheduleLine) using hardware instancing
* and clears the schedule.
*/
void DebugRenderer::drawScheduledLines()
{
    if (linesInstanceData.empty()) { return; }

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    InstancedRendering::drawInstanceData<InstanceDataLine>(linesInstanceData, line_VAO, line_instanceVBO, line_vertexCount, lineShaderProgram);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    linesInstanceData.clear();
}