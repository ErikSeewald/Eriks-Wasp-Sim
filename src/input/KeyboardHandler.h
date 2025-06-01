#pragma once

#include "Camera.h"

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