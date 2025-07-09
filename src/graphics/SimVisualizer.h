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
    const glm::vec3 upVector(0.0f, 1.0f, 0.0f);
    const glm::vec3 zeroVector(0.0f, 0.0f, 0.0f);

    /**
    * Initializes freeglut and ImGUI with all necessary parameters.
    */
    void init(int argc, char** argv);

    /**
    * The glut timer function for the SimVisualizer. Updates the display.
    */
    void timer(int value);

    /**
    * The reshape function for the SimVisualizer. Handles proper window-reshape modifications
    * and calls ImGui_ImplGLUT_ReshapeFunc because it overrides ImGui_ImplGLUT_InstallFuncs.
    */
    void reshape(int width, int height);

    /** 
    * Handles the GLUT window close event. Calls Console::freeTerminal().
    * On linux, if the application exits without freeTerminal(), the terminal 
    * will still be captured by readline after exiting.
    */
    void onWindowClose();

    /**
    * The main render method of the SimVisualizer. Other render calls branch from here.
    * Assumes that 'SimVisualizer::init' has already been called.
    */
    void render();

    /**
    * Updates the camera's position and matrices based on the current user inputs.
    */
    void updateCamera();

    const Camera& getCamera();
};
