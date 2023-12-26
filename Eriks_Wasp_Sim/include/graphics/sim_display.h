#include "GL/glut.h"
#include "GL/gl.h"
#include "wasp.h"

namespace sim_display 
{
    void updateCameraVectors();

    void keyboardDown(unsigned char key, int x, int y);

    void keyboardUp(unsigned char key, int x, int y);

    void specialKeyUp(int key, int x, int y);

    void specialKeyDown(int key, int x, int y);

    void updateKeys();

	void render();

    void drawGrid();

    void drawWasp(Wasp wasp);
}