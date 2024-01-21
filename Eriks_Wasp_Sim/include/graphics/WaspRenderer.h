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
    * @param wasp the std::list of wasps to visualize
    */
    void drawWasps(std::list<Wasp*>* wasps);

    /**
    * Initializes the WaspRenderer.
    */
    void init();

    /**
    * Loads the wasp model and binds it to VBO, VAO and EBO.
    */
    void _loadModel();
}