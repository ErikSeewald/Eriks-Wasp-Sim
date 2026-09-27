#include "DebugRenderer.h"
#include "SimVisualizer.h"
#include "ModelHandler.h"
#include "ShaderHandler.h"
#include "InstancedRendering.h"
#include <iostream>

using InstancedRendering::InstanceDataBasic;
using InstancedRendering::InstanceDataLine;

namespace DebugRenderer
{
    const glm::vec3 rootVec = glm::vec3(0, 0, 0);
    const glm::vec3 axisXVec = glm::vec3(10, 0, 0);
    const glm::vec3 axisYVec = glm::vec3(0, 10, 0);
    const glm::vec3 axisZVec = glm::vec3(0, 0, 10);

    const glm::vec4 axisXColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    const glm::vec4 axisYColor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
    const glm::vec4 axisZColor = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

    //MESH
    GLuint line_VAO;
    GLuint line_VBO;
    GLuint line_EBO;
    GLuint line_instanceVBO;
    int line_vertexCount;
    const std::string lineModelFile = "debug/Line.obj";

    GLuint roughSphere_VAO;
    GLuint roughSphere_VBO;
    GLuint roughSphere_EBO;
    GLuint roughSphere_instanceVBO;
    int roughSphere_vertexCount;
    const std::string roughSphereModelFile = "debug/RoughSphere.obj";

    //SHADER
    GLuint lineShaderProgram;
    const std::string lineVertShaderFile = "line.vert";
    const std::string colorFragShaderFile = "instance_color.frag";

    GLuint basicDebugShaderProgram;
    const std::string basicVertShaderFile = "instance_basic.vert";

    //INSTANCES
    std::vector<InstanceDataLine> linesInstanceData{};
    std::vector<InstanceDataLine> gridInstanceData{};

    /**
     * Initializes the instance data for drawing a grid of lines around the center.
     */
    void _initializeGridInstanceData()
    {
        constexpr glm::vec4 gridColor{ 0.4f, 0.4f, 0.4f, 1.0f };
        constexpr int gridSize = 10;
        float gridSizeFloat = (float) gridSize;
        gridInstanceData.reserve(gridSize * 6);

        // XY PLANE
        for (int i = 1; i <= gridSize; ++i)
        {
            const float x = (float) i;
            gridInstanceData.push_back({ {0.0f, x, 0.0f}, {gridSizeFloat, x, 0.0f}, gridColor});
            gridInstanceData.push_back({ {x, 0.0f, 0.0f}, {x, gridSizeFloat, 0.0f}, gridColor });
        }

        // YZ PLANE
        for (int i = 1; i <= gridSize; ++i)
        {
            const float y = (float) i;
            gridInstanceData.push_back({ {0.0f, y, 0.0f}, {0.0f, y, gridSizeFloat}, gridColor });
            gridInstanceData.push_back({ {0.0f, 0.0f, y}, {0.0f, gridSizeFloat, y}, gridColor });
        }

        // XZ PLANE
        for (int i = 1; i <= gridSize; ++i)
        {
            const float z = (float) i;
            gridInstanceData.push_back({ {z, 0.0f, 0.0f}, {z, 0.0f, gridSizeFloat}, gridColor });
            gridInstanceData.push_back({ {gridSizeFloat, 0.0f, z}, {0.0f, 0.0f, z}, gridColor });
        }
    }

    /**
    * Initializes the DebugRenderer. Loads models and builds shaders.
    */
    void init()
    {
        // LINE
        if (!ModelHandler::loadModel(lineModelFile, &line_VAO, &line_VBO, &line_EBO, &line_vertexCount))
        {
            std::cerr << "Failed to load line model" << std::endl;
            exit(EXIT_FAILURE);
        }
        InstancedRendering::setupInstancing<InstanceDataLine>(line_VAO, &line_instanceVBO);

        // ROUGH SPHERE
        if (!ModelHandler::loadModel(roughSphereModelFile, &roughSphere_VAO, &roughSphere_VBO, &roughSphere_EBO, &roughSphere_vertexCount))
        {
            std::cerr << "Failed to load roughSphere model" << std::endl;
            exit(EXIT_FAILURE);
        }
        InstancedRendering::setupInstancing<InstanceDataBasic>(roughSphere_VAO, &roughSphere_instanceVBO);

        // SHADERS
        basicDebugShaderProgram = ShaderHandler::buildShaderProgram(basicVertShaderFile, colorFragShaderFile);
        lineShaderProgram = ShaderHandler::buildShaderProgram(lineVertShaderFile, colorFragShaderFile);

        // GRID
        _initializeGridInstanceData();
    }

    /**
    * Draws a reference unit grid at the center of the coordinate system.
    */
    void drawGrid()
    {
        // GRID
        linesInstanceData.insert(
            linesInstanceData.end(),
            gridInstanceData.begin(),
            gridInstanceData.end()
        );

        // AXIS LINES
        scheduleLine(rootVec, axisXVec, axisXColor);
        scheduleLine(rootVec, axisYVec, axisYColor);
        scheduleLine(rootVec, axisZVec, axisZColor);
    }

    /**
    * Schedules a line to be drawn between the given start and end coordinates with the given color.
    * DebugRenderer::drawScheduledLines draws all previously scheduled lines at once
    * using instanced rendering.
    */
    void scheduleLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color)
    {
        linesInstanceData.push_back(InstanceDataLine{start, end, color});
    }

    /**
    * Draws previously scheduled lines (DebugRenderer::scheduleLine) using hardware instancing
    * and clears the schedule.
    */
    void drawScheduledLines()
    {
        if (linesInstanceData.empty()) { return; }

        InstancedRendering::drawInstanceData<InstanceDataLine>(linesInstanceData, line_VAO, line_instanceVBO, 
            line_vertexCount, lineShaderProgram, GL_LINES);

        linesInstanceData.clear();
    }

    /**
     * Draws a rough approximation of a sphere wireframe at the given position with the given radius.
     */
    void drawRoughSphere(const glm::vec3& position, float radius, const glm::vec4& color)
    {
        // InstancedRendering needs to interpret it as GL_TRIANGLES for correct mesh order while
        // glPolygonMode needs GL_LINE for drawing the wireframe.
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        std::vector<InstanceDataBasic> singleInstanceData(1, InstanceDataBasic{ position, color, radius});
        InstancedRendering::drawInstanceData(singleInstanceData, roughSphere_VAO, roughSphere_instanceVBO, 
            roughSphere_vertexCount, basicDebugShaderProgram, GL_TRIANGLES);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}