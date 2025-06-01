#include "SimVisualizer.h"
#include "KeyboardHandler.h"
#include "MouseHandler.h"
#include "WaspRenderer.h"
#include "FoodRenderer.h"
#include "UI.h"
#include "DebugRenderer.h"
#include "Console.h"
#include <iostream>
#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glut.h"
#include "glm/gtc/matrix_transform.hpp"
#include "GL/freeglut.h"

Camera camera;

/**
* Initializes freeglut and ImGUI with all necessary parameters.
*/
void SimVisualizer::init(int argc, char** argv)
{
    glutInit(&argc, argv);

    glutInitWindowSize(1280, 720);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow("Eriks Wasp Sim");

    //GLEW
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Error: glewInit failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    // IMGUI
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGLUT_Init();
    ImGui_ImplGLUT_InstallFuncs();
    ImGui_ImplOpenGL3_Init("#version 130");
    UI::getUIState()->drawGrid = true;

    //EVENT HANDLERS
    //(Overwrite the ones previously installed by ImGui_ImplGLUT_InstallFuncs().
    // If the ImGui_ImplGLUT function is also needed they need to call it themselves)
    glutKeyboardFunc(KeyboardHandler::keyboardDown);
    glutKeyboardUpFunc(KeyboardHandler::keyboardUp);

    glutSpecialFunc(KeyboardHandler::specialKeyDown);
    glutSpecialUpFunc(KeyboardHandler::specialKeyUp);

    glutMouseFunc(MouseHandler::mouseClick);

    glutDisplayFunc(SimVisualizer::render);
    glutReshapeFunc(SimVisualizer::reshape);
    glutCloseFunc(SimVisualizer::onWindowClose);

    //CAMERA
    camera.position = glm::vec3(3.0f, 3.0f, 3.0f);
    camera.direction = glm::vec3(-1.0f, 0.0f, -1.0f);
    updateCamera();

    //RENDERERS
    DebugRenderer::init();
    WaspRenderer::init();
    FoodRenderer::init();
    
    glutTimerFunc(0, timer, 0);
    glutMainLoop();
}

/**
* The glut timer function for the SimVisualizer. Updates the display.
*/
void SimVisualizer::timer(int value)
{
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

/**
* The reshape function for the SimVisualizer. Handles proper window-reshape modifications
* and calls ImGui_ImplGLUT_ReshapeFunc because it overrides ImGui_ImplGLUT_InstallFuncs.
*/
void SimVisualizer::reshape(int width, int height)
{
    glViewport(0, 0, width, height);
    ImGui_ImplGLUT_ReshapeFunc(width, height);
}

/** 
* Handles the GLUT window close event. Calls Console::freeTerminal().
* On linux, if the application exits without freeTerminal(), the terminal 
* will still be captured by readline after exiting.
*/
void SimVisualizer::onWindowClose()
{
    Console::freeTerminal();
}

/**
* The main render method of the SimVisualizer. Other render calls branch from here.
* Assumes that 'SimVisualizer::init' has already been called.
*/
void SimVisualizer::render()
{ 
    UI::UI_STATE* uiState = UI::getUIState();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    // SCENE
    SimVisualizer::updateCamera();

    if (uiState->drawGrid) { DebugRenderer::drawGrid(); }

    FoodRenderer::drawFood(*Food::getFoodEntities());
    WaspRenderer::drawWasps(*WaspSlots::getWasps());
    WaspRenderer::drawSelectedWasp();

    DebugRenderer::drawScheduledLines();
 
    // UI
    UI::drawUI();

    glutSwapBuffers();
}

/**
* Updates the camera's position and matrices based on the current user inputs.
*/
void SimVisualizer::updateCamera()
{
    KeyboardHandler::updateCamera(&camera);

    camera.direction = glm::vec3(
        cos(camera.yawRad) * cos(camera.pitchRad),
        sin(camera.pitchRad),
        sin(camera.yawRad) * cos(camera.pitchRad)
    );

    camera.view = glm::lookAt(
        camera.position,
        camera.position + camera.direction,
        SimVisualizer::upVector
    );

    camera.projection = glm::perspective(
        CameraSettings::FOV_RADIANS,
        (float) glutGet(GLUT_WINDOW_WIDTH) / (float) glutGet(GLUT_WINDOW_HEIGHT),
        CameraSettings::NEAR_CLIP,
        CameraSettings::FAR_CLIP
    );
}

const Camera& SimVisualizer::getCamera()
{
    return camera;
}