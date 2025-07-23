#pragma once

#include "SimVisualizer.h"
#include <vector>

/**
* @namespace InstancedRendering
* @brief A namespace for methods related to hardware instanced rendering.
*/
namespace InstancedRendering
{
    /**
    * Binds the given VAO and instanceVBO to GL and expands the VertexAttribArray to enable hardware instancing
    * for the corresponding instance data template.
    */
    template <typename T>
    void setupInstancing(GLuint VAO, GLuint* instanceVBO);

    /**
    * Draws the given instance data using hardware instancing with the given modelVAO, instanceVBO,
    * vertexCount and shaderProgram. Assumes ShaderHandler::setupInstancing has been called beforehand.
    */
    template <typename T>
    void drawInstanceData(const std::vector<T>& instanceData, GLuint modelVAO, GLuint instanceVBO, int indexCount, GLuint shaderProgram);

    /**
    * Small instance data struct with only a position and color attribute for each instance.
    */
    struct InstanceDataBasic
    {
        glm::vec3 position;
        glm::vec4 color;
    };

    /**
    * Instance data struct for a line with start and end points as well as a color.
    */
    struct InstanceDataLine
    {
        glm::vec3 start;
        glm::vec3 end;
        glm::vec4 color;
    };

    /**
    * Small instance data struct with only position and viewing vector attributes.
    * Designed for wasp instances that get rotated based on the viewing vector by the shader.
    */
    struct InstanceDataWasp
    {
        glm::vec3 position;
        glm::vec3 viewingVector;
    };
}