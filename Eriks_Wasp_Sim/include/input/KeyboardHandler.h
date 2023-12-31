/**
 * @file KeyboardHandler.h
 * @brief This file serves as the header for Keyboardhandler.cpp.
 */

#pragma once

#include "SimVisualizer.h"
#include "Camera.h"
#include <DirectXMath.h>
using DirectX::XMFLOAT3;

/**
* @namespace KeyboardHandler
* @brief A namespace for all methods to handle keyboard input from the user.
*/
namespace KeyboardHandler 
{
    /**
    * Updates the given Camera position and tilt based on the registered user inputs.
    * 
    * @param camera - the Camera to update
    */
    void updateCamera(Camera* camera);


    /**
    * Handles all GLUT keyboard down events.
    */
	void keyboardDown(unsigned char key, int x, int y);

    /**
    * Handles all GLUT keyboard up events.
    */
    void keyboardUp(unsigned char key, int x, int y);

    /**
    * Handles all special key (i.e. arrow keys) GLUT keyboard down events.
    */
    void specialKeyDown(int key, int x, int y);

    /**
    * Handles all special key (i.e. arrow keys) GLUT keyboard up events.
    */
    void specialKeyUp(int key, int x, int y);
};