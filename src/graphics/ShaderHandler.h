#pragma once

#include "SimVisualizer.h"
#include "glm/ext.hpp"
#include <string>

 /**
 * @namespace ShaderHandler
 * @brief A namespace for util methods for handling vertex and fragment shaders.
 */
namespace ShaderHandler
{
    /**
    * Loads a shader file into a string and returns it.
    * Assumes the file is located in the default shader directory.
    */
    std::string loadShaderFile(const std::string& fileName);

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
    * Loads the given shader files (assumed to be in the default shader directory), compiles them
    * and then links the shader program.
    * Calls exit(EXIT_FAILURE) if the shader program fails to build.
    * 
    * @return the GL ID of the shader program
    */
    GLuint buildShaderProgram(const std::string& vertShaderFile, const std::string& fragShaderFile);
}