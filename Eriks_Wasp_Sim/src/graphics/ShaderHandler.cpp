#include "ShaderHandler.h"
#include "glm/ext.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

/**
 * Loads a shader file into a string and returns it.
 */
std::string ShaderHandler::loadShaderFile(const std::string& filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open shader: " + filePath);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

/**
 * Compiles the GLSL shader from the given source code (string returned by loadShaderFile()).
 * @return the GL shader ID
 */
GLuint ShaderHandler::compileShader(GLenum shaderType, const std::string& source)
{
    GLuint shaderID = glCreateShader(shaderType);

    const char* srcPtr = source.c_str();
    glShaderSource(shaderID, 1, &srcPtr, nullptr);
    glCompileShader(shaderID);

    // CHECK FOR COMPILE ERRORS
    GLint success;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[1024];
        glGetShaderInfoLog(shaderID, 1024, nullptr, infoLog);
        std::string errMsg = "Shader compile error:\n";
        errMsg += infoLog;
        throw std::runtime_error(errMsg);
    }

    return shaderID;
}

/**
* Links a shader program from a pre-compiled vertex shader and fragment shader.
*
* @return the GL ID of the shader program
*/
GLuint ShaderHandler::linkShaderProgram(GLuint vertShaderID, GLuint fragShaderID)
{
    GLuint programID = glCreateProgram();
    glAttachShader(programID, vertShaderID);
    glAttachShader(programID, fragShaderID);

    glLinkProgram(programID);

    // CHECK FOR LINK ERRORS
    GLint success;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[1024];
        glGetProgramInfoLog(programID, 1024, nullptr, infoLog);
        std::string errMsg = "Program link error:\n";
        errMsg += infoLog;
        throw std::runtime_error(errMsg);
    }

    // DETACH AFTER LINKING
    glDetachShader(programID, vertShaderID);
    glDetachShader(programID, fragShaderID);

    return programID;
}

/**
* Draws the given model instance data using hardware instancing based on the given modelVAO, instanceVBO,
* vertexCount and shaderProgram to use.
*/
void ShaderHandler::drawInstanceData(std::vector<glm::mat4>* instanceData, GLuint* modelVAO, GLuint* instanceVBO, 
    int vertexCount, GLuint* shaderProgram)
{
    // UPLOAD INSTANCE DATA TO GPU
    glBindBuffer(GL_ARRAY_BUFFER, *instanceVBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        instanceData->size() * sizeof(glm::mat4),
        instanceData->data(),
        GL_DYNAMIC_DRAW
    );

    // USE SHADER PROGRAM WITH CAMERA TRANSFORM
    glUseProgram(*shaderProgram);
    glUniformMatrix4fv(
        glGetUniformLocation(*shaderProgram, "uViewProj"),
        1,
        GL_FALSE,
        glm::value_ptr(SimVisualizer::getCamProjMatrix())
    );

    // ISSUE SINGLE DRAW CALL
    glBindVertexArray(*modelVAO);
    glDrawElementsInstanced(
        GL_TRIANGLES,
        vertexCount,
        GL_UNSIGNED_INT,
        0,
        static_cast<GLsizei>(instanceData->size())
    );

    glBindVertexArray(0);
    glUseProgram(0);
}