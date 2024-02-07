#pragma once

#include "Wasp.h"
#include <list>

 /**
 * @namespace WaspRenderer
 * @brief A namespace for all methods for rendering Wasps with OpenGL
 */
namespace WaspRenderer
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
    * Visualizes the given list of wasps. Assumes glut, glew, etc. are preinitialized.
    *
    * @param wasps the std::list of wasps to visualize
    */
    void drawWasps(std::list<Wasp*>* wasps);

    /**
    * Visualizes the wasp selected by the user, provided it exists.
    */
    void drawSelectedWasp();
#
    /**
    * Draws the given wasp with the assumption that the correct vertex array has already been bound and the
    * color has been set. This allows for the function to only contain code unique to each wasp being drawn in
    * drawWasps()'s loop and thereby increases performance.
    *
    * @param wasp the wasp to draw
    */
    void _drawWaspPrebound(Wasp* wasp);

    /**
    * Initializes the WaspRenderer.
    */
    void init();

    /**
    * Loads the wasp model and binds it to VBO, VAO and EBO.
    */
    void _loadModel();
}