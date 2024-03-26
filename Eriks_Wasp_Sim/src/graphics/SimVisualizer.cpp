#pragma once

#include "SimVisualizer.h"
#include "KeyboardHandler.h"
#include "MouseHandler.h"
#include "WaspRenderer.h"
#include "FoodRenderer.h"
#include "UI.h"
#include <iostream>
#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glut.h"

Camera camera;

/**
* Initializes freeglut with all necessary parameters.
*/
void SimVisualizer::initGlut(int argc, char** argv)
{
    glutInit(&argc, argv);

    glutInitWindowSize(1280, 720);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutCreateWindow("Eriks Wasp Sim");
    glutDisplayFunc(SimVisualizer::render);
    glutReshapeFunc(SimVisualizer::reshape);

    //GLEW
    glewExperimental = GL_TRUE;
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

    //EVENT HANDLERS
    glutKeyboardFunc(KeyboardHandler::keyboardDown);
    glutKeyboardUpFunc(KeyboardHandler::keyboardUp);

    glutSpecialFunc(KeyboardHandler::specialKeyDown);
    glutSpecialUpFunc(KeyboardHandler::specialKeyUp);

    glutMouseFunc(MouseHandler::mouseClick);

    //CAMERA
    camera.position = glm::vec3(3.0f, 3.0f, 3.0f);
    camera.direction = glm::vec3(-1.0f, 0.0f, -1.0f);
    updateCameraVectors();

    //WASP RENDERER
    WaspRenderer::init();
 
    glutTimerFunc(0, timer, 0);
    glutMainLoop();
}

/**
* The glut timer function for the SimVisualizer. Updates the display and input handlers.
*/
void SimVisualizer::timer(int value)
{
    KeyboardHandler::updateCamera(&camera);
    glutPostRedisplay();

    // call timer again after delay
    glutTimerFunc(16, timer, 0); // around 60 FPS
}

/**
* The glut reshape function for the SimVisualizer. Handles proper window-reshape modifications.
*/
void SimVisualizer::reshape(int width, int height)
{
    if (height == 0) height = 1;
    float aspect_ratio = (float) width / (float) height;

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, aspect_ratio, 0.01f, 500.0);
}


/**
* The main render method of the SimVisualizer. Other render calls branch from here.
* Assumes that 'SimVisualizer::initGlut' has already been called.
*/
void SimVisualizer::render()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // CAMERA
    reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
    glEnable(GL_DEPTH_TEST);
    updateGlutCamera();

    // SCENE
    drawGrid();

    FoodRenderer::drawFood(Food::getFoodSlots());

    WaspRenderer::drawWasps(WaspSlots::getWaspSlots());
    WaspRenderer::drawSelectedWasp();
 
    // UI
    UI::drawUI();

    glutSwapBuffers();
}


/**
* Updates the glut 'camera' (gluLookAt) with the camera struct's attributes.
*/
void SimVisualizer::updateGlutCamera()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(camera.position.x, camera.position.y, camera.position.z,  // Camera position
        camera.position.x + camera.direction.x, camera.position.y + camera.direction.y, camera.position.z + camera.direction.z, // Viewing direction
        0.0, 1.0, 0.0); // Head is up
}


/**
* Updates the Cameras vectors based on its position, direction, pitch and yaw.
*/
void SimVisualizer::updateCameraVectors()
{
    static float pi = 3.14159265359f;

    float radianYaw = camera.yaw * pi / 180.0;
    float radianPitch = camera.pitch * pi / 180.0;
    camera.direction.x = cos(radianYaw) * cos(radianPitch);
    camera.direction.y = sin(radianPitch);
    camera.direction.z = sin(radianYaw) * cos(radianPitch);
}

Camera SimVisualizer::getCamera()
{
    return camera;
}


/**
* Draws a 3D line between the given start and end points.
*
* @param start - the vec3 start of the line
* @param end - the vec3 end of the line
*/
void SimVisualizer::drawBasicLine(glm::vec3 start, glm::vec3 end)
{
    glBegin(GL_LINES);
    glVertex3f(start.x, start.y, start.z);
    glVertex3f(end.x, end.y, end.z);
    glEnd();
}

/**
* Debug helper method. Draws a grid around the center of the coordinate system.
*/
void SimVisualizer::drawGrid() {

    glPushMatrix();

    //X Axis
    glColor3f(1.0f, 0.0f, 0.0f);
    drawBasicLine(SimVisualizer::zeroVector, glm::vec3(10.0f, 0.0f, 0.0f));

    //Y Axis
    glColor3f(0.0f, 1.0f, 0.0f);
    drawBasicLine(SimVisualizer::zeroVector, glm::vec3(0.0f, 10.0f, 0.0f));

    //Z Axis
    glColor3f(0.0f, 0.0f, 1.0f);
    drawBasicLine(SimVisualizer::zeroVector, glm::vec3(00.0f, 0.0f, 10.0f));

    //GRID
    glColor3f(0.3f, 0.3f, 0.3f);

    //X - Y Plane
    for (float i = 1.0f; i <= 10.0f; i += 1.0f)
    {
        drawBasicLine(glm::vec3(0.0f, i, 0.0f), glm::vec3(10.0f, i, 0.0f));
    }

    for (float i = 1.0f; i <= 10.0f; i += 1.0f)
    {
        drawBasicLine(glm::vec3(i, 0.0f, 0.0f), glm::vec3(i, 10.0f, 0.0f));
    }

    //Z - Y Plane
    for (float i = 1.0f; i <= 10.0f; i += 1.0f)
    {
        drawBasicLine(glm::vec3(0.0f, i, 0.0f), glm::vec3(0.0f, i, 10.0f));
    }

    for (float i = 1.0f; i <= 10.0f; i += 1.0f)
    {
        drawBasicLine(glm::vec3(0.0f, 0.0f, i), glm::vec3(0.0f, 10.0f, i));
    }

    //X - Z Plane
    for (float i = 1.0f; i <= 10.0f; i += 1.0f)
    {
        drawBasicLine(glm::vec3(i, 0.0f, 0.0f), glm::vec3(i, 0.0f, 10.f));
    }

    for (float i = 1.0f; i <= 10.0f; i += 1.0f)
    {
        drawBasicLine(glm::vec3(0.0f, 0.0f, i), glm::vec3(10.0f, 0.0f, i));
    }

    glPopMatrix();
}

/**
* Converts degrees to radians
*
* @return converted radians
*/
float SimVisualizer::degToRad(float degrees)
{
    static const float c = 3.14159265359f / 180.0f;
    return degrees * c;
}

/**
* Converts radians to degrees
*
* @return converted degrees
*/
float SimVisualizer::radToDeg(float radians)
{
    static const float c = 180.0f / 3.14159265359f;
    return radians * c;
}