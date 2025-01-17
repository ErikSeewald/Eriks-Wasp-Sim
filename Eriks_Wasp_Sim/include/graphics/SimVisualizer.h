#pragma once

#include <GL/glew.h>
#include <GL/glut.h>
#include <glm/glm.hpp>

#include "Camera.h"

/**
* @namespace SimVisualizer
* @brief A namespace for all methods for visualizing the simulation with OpenGL 
*/
namespace SimVisualizer
{
    const glm::vec3 zeroVector(0.0f, 0.0f, 0.0f);

    /**
    * Initializes freeglut with all necessary parameters.
    */
    void initGlut(int argc, char** argv);

    /**
    * The glut idle function for the SimVisualizer. Updates the display and input handlers. Makes use of vsync.
    */
    void idleUpdate();

    /**
    * The glut reshape function for the SimVisualizer. Handles proper window-reshape modifications.
    */
    void reshape(int width, int height);

    /**
    * The main render method of the SimVisualizer. Other render calls branch from here.
    * Assumes that 'SimVisualizer::initGlut' has already been called.
    */
    void render();

    /**
    * Updates the glut 'camera' (gluLookAt) with the camera struct's attributes.
    * Used by slower direct graphics functions. Shader graphics functions make use of getCamProjMatrix().
    */
    void updateGlutCamera();

    /**
    * Updates the Cameras vectors based on its position, direction, pitch and yaw.
    */
    void updateCameraVectors();

    Camera getCamera();

    glm::mat4 getCamProjMatrix();

    /**
    * Draws a 3D line between the given start and end points.
    * 
    * @param start - the vec3 start of the line
    * @param end - the vec3 end of the line
    */
    void drawBasicLine(glm::vec3 start, glm::vec3 end);

    /**
    * Debug helper method. Draws a grid around the center of the coordinate system.
    */
    void drawGrid();

    /**
    * Converts degrees to radians
    * 
    * @return converted radians
    */
    float degToRad(float degrees);

    /**
    * Converts radians to degrees
    *
    * @return converted degrees
    */
    float radToDeg(float radians);
};