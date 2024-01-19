/**
 * @file SimVisualizer.cpp
 * @brief This file specifies the implementation of SimVisualizer.h
 */

#include "SimVisualizer.h"
#include "KeyboardHandler.h"
#include "Camera.h"
#include <list>
#include "Simulation.h"
#include "WaspRenderer.h"
#include <iostream>
#include <cstdlib>

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

    glEnable(GL_DEPTH_TEST);

    glutDisplayFunc(SimVisualizer::render);
    glutReshapeFunc(SimVisualizer::reshape);

    glutKeyboardFunc(KeyboardHandler::keyboardDown);
    glutKeyboardUpFunc(KeyboardHandler::keyboardUp);

    glutSpecialFunc(KeyboardHandler::specialKeyDown);
    glutSpecialUpFunc(KeyboardHandler::specialKeyUp);

    camera.position = XMFLOAT3(3.0f, 3.0f, 3.0f);
    camera.direction = XMFLOAT3(-1.0f, 0.0f, -1.0f);
    updateCameraVectors();

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Error: glewInit failed" << std::endl;
        exit(EXIT_FAILURE);
    }

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

    updateGlutCamera();

    drawGrid();

    WaspRenderer::drawWasps(Simulation::getWasps());
    
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


/**
* Draws a 3D line between the given start and end points.
*
* @param start - the XMFLOAT3 start of the line
* @param end - the XMFLOAT3 end of the line
*/
void SimVisualizer::drawBasicLine(XMFLOAT3 start, XMFLOAT3 end)
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
    drawBasicLine(SimVisualizer::zeroVector, XMFLOAT3(10.0f, 0.0f, 0.0f));

    //Y Axis
    glColor3f(0.0f, 1.0f, 0.0f);
    drawBasicLine(SimVisualizer::zeroVector, XMFLOAT3(0.0f, 10.0f, 0.0f));

    //Z Axis
    glColor3f(0.0f, 0.0f, 1.0f);
    drawBasicLine(SimVisualizer::zeroVector, XMFLOAT3(00.0f, 0.0f, 10.0f));

    //GRID
    glColor3f(0.3f, 0.3f, 0.3f);

    //X - Y Plane
    for (float i = 1.0f; i <= 10.0f; i += 1.0f)
    {
        drawBasicLine(XMFLOAT3(0.0f, i, 0.0f), XMFLOAT3(10.0f, i, 0.0f));
    }

    for (float i = 1.0f; i <= 10.0f; i += 1.0f)
    {
        drawBasicLine(XMFLOAT3(i, 0.0f, 0.0f), XMFLOAT3(i, 10.0f, 0.0f));
    }

    //Z - Y Plane
    for (float i = 1.0f; i <= 10.0f; i += 1.0f)
    {
        drawBasicLine(XMFLOAT3(0.0f, i, 0.0f), XMFLOAT3(0.0f, i, 10.0f));
    }

    for (float i = 1.0f; i <= 10.0f; i += 1.0f)
    {
        drawBasicLine(XMFLOAT3(0.0f, 0.0f, i), XMFLOAT3(0.0f, 10.0f, i));
    }

    //X - Z Plane
    for (float i = 1.0f; i <= 10.0f; i += 1.0f)
    {
        drawBasicLine(XMFLOAT3(i, 0.0f, 0.0f), XMFLOAT3(i, 0.0f, 10.f));
    }

    for (float i = 1.0f; i <= 10.0f; i += 1.0f)
    {
        drawBasicLine(XMFLOAT3(0.0f, 0.0f, i), XMFLOAT3(10.0f, 0.0f, i));
    }

    glPopMatrix();
}