/**
 * @file Wasp.h
 * @brief This file serves as the header for SimVisualizer.cpp.
 */

#pragma once

#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include "Wasp.h"
#include <DirectXMath.h>

using DirectX::XMFLOAT3;

/**
* @namespace SimVisualizer
* @brief A namespace for all methods for visualizing the simulation with OpenGL 
*/
namespace SimVisualizer
{
    /**
    * Initializes freeglut with all necessary parameters.
    */
    void initGlut(int argc, char** argv);

    /**
    * The glut timer function for the SimVisualizer. Updates the display and input handlers.
    */
    void timer(int value);

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
    */
    void updateGlutCamera();

    /**
    * Updates the Cameras vectors based on its position, direction, pitch and yaw.
    */
    void updateCameraVectors();

    /**
    * Draws a 3D line between the given start and end points.
    * 
    * @param start - the XMFLOAT3 start of the line
    * @param end - the XMFLOAT3 end of the line
    */
    void drawBasicLine(XMFLOAT3 start, XMFLOAT3 end);

    /**
    * Debug helper method. Draws a grid around the center of the coordinate system.
    */
    void drawGrid();
};