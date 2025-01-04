#pragma once

#include "SimVisualizer.h"
#include <vector>
#include <string>

 /**
 * @namespace ModelHandler
 * @brief A namespace for util methods for handling 3D Models for the Simulation.
 */
namespace ModelHandler
{
    /**
    * @struct Vertex
    * @brief Struct to hold position, normal and texture coordinate information of a mesh vertex
    */
    struct Vertex {
        float position[3];
        float normal[3];
        float texcoord[2];
    };

    /**
    * Loads the wasp model fileName from the given base directory, binds it to VAO, VBO, EBO and writes
    * the vertex count of the model into vertexCount.
    *
    * @return bool - success of the operation
    */
    bool loadModel(const std::string& baseDir, const std::string& fileName, GLuint* VAO, GLuint* VBO, GLuint* EBO, 
        int* vertexCount);
}