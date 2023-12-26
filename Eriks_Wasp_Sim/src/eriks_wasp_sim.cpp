#include "eriks_wasp_sim.h"

void reshape(int width, int height) {

    if (height == 0) height = 1;
    float aspect_ratio = (float) width / (float) height;

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, aspect_ratio, 1.0, 100.0);
}

void timer(int value) 
{
    sim_display::updateKeys();

    // call timer again after delay
    glutTimerFunc(16, timer, 0); // around 60 FPS
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);

    glutInitWindowSize(1280, 720);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow("Eriks Wasp Sim");

    glEnable(GL_DEPTH_TEST);

    glutDisplayFunc(sim_display::render);
    glutReshapeFunc(reshape);

    glutKeyboardFunc(sim_display::keyboardDown);
    glutKeyboardUpFunc(sim_display::keyboardUp);

    glutSpecialFunc(sim_display::specialKeyDown);
    glutSpecialUpFunc(sim_display::specialKeyUp);

    glutTimerFunc(0, timer, 0); // start timer

    glutMainLoop();
    return 0;
}