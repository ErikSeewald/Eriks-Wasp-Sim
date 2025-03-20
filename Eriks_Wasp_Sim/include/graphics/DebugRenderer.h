#include <glm/glm.hpp>

/**
* @namespace DebugRenderer
* @brief A namespace for methods related to rendering debug information
*/
namespace DebugRenderer
{
    /**
    * Initializes the DebugRenderer. Loads models and builds shaders.
    */
    void init();

    /**
    * Draws a reference unit grid at the center of the coordinate system.
    */
    void drawGrid();

    /**
    * Schedules a line to be drawn between the given start and end coordinates with the given color.
    * DebugRenderer::drawScheduledLines draws all previously scheduled lines at once
    * using instanced rendering.
    */
    void scheduleLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color);

    /**
    * Draws previously scheduled lines (DebugRenderer::scheduleLine) using hardware instancing
    * and clears the schedule.
    */
    void drawScheduledLines();
}