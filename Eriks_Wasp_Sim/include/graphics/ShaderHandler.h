#pragma once

#include "SimVisualizer.h"
#include <vector>
#include <string>

 /**
 * @namespace ShaderHandler
 * @brief A namespace for util methods for handling vertex and fragment shaders.
 */
namespace ShaderHandler
{
    /**
     * Loads a shader file into a string and returns it.
     */
    std::string loadShaderFile(const std::string& filePath);

    /**
     * Compiles the GLSL shader from the given source code (string returned by loadShaderFile()).
     * @return the GL shader ID
     */
    GLuint compileShader(GLenum shaderType, const std::string& source);

    /**
    * Links a shader program from a pre-compiled vertex shader and fragment shader.
    *
    * @return the GL ID of the shader program
    */
    GLuint linkShaderProgram(GLuint vertShaderID, GLuint fragShaderID);

    /**
    * Draws the given model instance data using hardware instancing based on the given modelVAO, instanceVBO, 
    * vertexCount and shaderProgram to use.
    */
    void drawInstanceData(std::vector<glm::mat4>* instanceData, GLuint* modelVAO, GLuint* instanceVBO,
        int vertexCount, GLuint* shaderProgram);
}