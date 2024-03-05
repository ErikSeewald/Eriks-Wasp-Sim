#include "KeyboardHandler.h"
#include "SimVisualizer.h"
#include "UI.h"

bool keyStates[256];
bool specialKeyStates[256];

/**
* Updates the given Camera position and tilt based on the registered user inputs.
*
* @param camera - the Camera to update
*/
void KeyboardHandler::updateCamera(Camera* camera)
{
    static float cameraSpeed = 0.1f;
    static float angleSpeed = 2.0f;

    if (keyStates['w'])
    {
        camera->position.x += camera->direction.x * cameraSpeed;
        camera->position.y += camera->direction.y * cameraSpeed;
        camera->position.z += camera->direction.z * cameraSpeed;
    }
    if (keyStates['s'])
    {
        camera->position.x -= camera->direction.x * cameraSpeed;
        camera->position.y -= camera->direction.y * cameraSpeed;
        camera->position.z -= camera->direction.z * cameraSpeed;
    }
    if (keyStates['a'])
    {
        camera->position.x += camera->direction.z * cameraSpeed;
        camera->position.z -= camera->direction.x * cameraSpeed;
    }

    if (keyStates['d'])
    {
        camera->position.x -= camera->direction.z * cameraSpeed;
        camera->position.z += camera->direction.x * cameraSpeed;
    }

    if (specialKeyStates[GLUT_KEY_UP])
    {
        camera->pitch += angleSpeed;
        if (camera->pitch > 89.0f) camera->pitch = 89.0f;
    }

    if (specialKeyStates[GLUT_KEY_DOWN])
    {
        camera->pitch -= angleSpeed;
        if (camera->pitch < -89.0f) camera->pitch = -89.0f;
    }

    if (specialKeyStates[GLUT_KEY_LEFT])
    {
        camera->yaw -= angleSpeed;
    }

    if (specialKeyStates[GLUT_KEY_RIGHT])
    {
        camera->yaw += angleSpeed;
    }

    if (specialKeyStates[GLUT_KEY_RIGHT] || specialKeyStates[GLUT_KEY_LEFT] || specialKeyStates[GLUT_KEY_DOWN] || specialKeyStates[GLUT_KEY_UP])
    {
        SimVisualizer::updateCameraVectors();
    } 
}


/**
* Handles all GLUT keyboard down events.
*/
void KeyboardHandler::keyboardDown(unsigned char key, int x, int y)
{
    // PAUSE/UNPAUSE
    if (key == 'p')
    {
        UI::getUIState()->isPaused = !UI::getUIState()->isPaused;
    }

    keyStates[key] = true;
}

/**
* Handles all GLUT keyboard up events.
*/
void KeyboardHandler::keyboardUp(unsigned char key, int x, int y)
{
    keyStates[key] = false;
}

/**
* Handles all special key (i.e. arrow keys) GLUT keyboard down events.
*/
void KeyboardHandler::specialKeyDown(int key, int x, int y)
{
    specialKeyStates[key] = true;
}

/**
* Handles all special key (i.e. arrow keys) GLUT keyboard up events.
*/
void KeyboardHandler::specialKeyUp(int key, int x, int y)
{
    specialKeyStates[key] = false;
}