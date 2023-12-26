#include "sim_display.h"

#define PI 3.14159265359

XMFLOAT3 cameraPos(3.0f, 3.0f, 3.0f);
XMFLOAT3 cameraDir(-1.0f, 0.0f, -1.0f);

float yaw = -130.0f;
float pitch = -30.0f;

bool keyStates[256];
bool specialKeyStates[256];

void sim_display::updateCameraVectors() 
{
    float radianYaw = yaw * PI/180.0;
    float radianPitch = pitch * PI / 180.0;
    cameraDir.x = cos(radianYaw) * cos(radianPitch);
    cameraDir.y = sin(radianPitch);
    cameraDir.z = sin(radianYaw) * cos(radianPitch);
}

void sim_display::keyboardDown(unsigned char key, int x, int y) 
{
    keyStates[key] = true;
}

void sim_display::keyboardUp(unsigned char key, int x, int y) 
{
    keyStates[key] = false;
}

void sim_display::specialKeyDown(int key, int x, int y) 
{
    specialKeyStates[key] = true;
}

void sim_display::specialKeyUp(int key, int x, int y) 
{
    specialKeyStates[key] = false;
}

void sim_display::updateKeys()
{
    float cameraSpeed = 0.1f;
    float angleSpeed = 2.0f;

    if (keyStates['w'])
    {
        cameraPos.x += cameraDir.x * cameraSpeed;
        cameraPos.y += cameraDir.y * cameraSpeed;
        cameraPos.z += cameraDir.z * cameraSpeed;
    }
    if (keyStates['s'])
    {
        cameraPos.x -= cameraDir.x * cameraSpeed;
        cameraPos.y -= cameraDir.y * cameraSpeed;
        cameraPos.z -= cameraDir.z * cameraSpeed;
    }
    if (keyStates['a'])
    {
        cameraPos.x += cameraDir.z * cameraSpeed;
        cameraPos.z -= cameraDir.x * cameraSpeed;
    }

    if (keyStates['d'])
    {
        cameraPos.x -= cameraDir.z * cameraSpeed;
        cameraPos.z += cameraDir.x * cameraSpeed;
    }

    if (specialKeyStates[GLUT_KEY_UP])
    {
        pitch += angleSpeed;
        if (pitch > 89.0f) pitch = 89.0f;
    }

    if (specialKeyStates[GLUT_KEY_DOWN])
    {
        pitch -= angleSpeed;
        if (pitch < -89.0f) pitch = -89.0f;
    }

    if (specialKeyStates[GLUT_KEY_LEFT])
    {
        yaw -= angleSpeed;
    }

    if (specialKeyStates[GLUT_KEY_RIGHT])
    {
        yaw += angleSpeed;
    }

    updateCameraVectors();
    glutPostRedisplay();
}

void sim_display::render()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    updateCameraVectors();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(cameraPos.x, cameraPos.y, cameraPos.z,  // Camera position
        cameraPos.x + cameraDir.x, cameraPos.y + cameraDir.y, cameraPos.z + cameraDir.z, // Viewing direction
        0.0, 1.0, 0.0); // Head is up

    drawGrid();

    Wasp wasp = Wasp();
    wasp.setPosition(XMFLOAT3(0.0f, 1.0f, 1.0f));
    drawWasp(wasp);

    glutSwapBuffers();
}

void sim_display::drawWasp(Wasp wasp)
{
    glColor3f(1.0f, 0.5f, 0.0f); 
    glTranslatef(wasp.getPosition().x, wasp.getPosition().y, wasp.getPosition().z);
    glutSolidCube(0.2);
}

void drawBasicLine(XMFLOAT3 start, XMFLOAT3 end)
{
    glBegin(GL_LINES);
    glVertex3f(start.x, start.y, start.z);
    glVertex3f(end.x, end.y, end.z);
    glEnd();
}

void sim_display::drawGrid() {

    //X Axis
    glColor3f(1.0f, 0.0f, 0.0f);
    drawBasicLine(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(10.0f, 0.0f, 0.0f));

    //Y Axis
    glColor3f(0.0f, 1.0f, 0.0f);
    drawBasicLine(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 10.0f, 0.0f));

    //Z Axis
    glColor3f(0.0f, 0.0f, 1.0f);
    drawBasicLine(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(00.0f, 0.0f, 10.0f));

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

}