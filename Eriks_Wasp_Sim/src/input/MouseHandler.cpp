#include "MouseHandler.h"

#include <GL/glut.h>
#include "UI.h"
#include "MouseRayCast.h"

void MouseHandler::mouseClick(int button, int state, int x, int y)
{
    UI::UI_STATE* uiState = UI::getUIState();

    //LEFT CLICK
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) 
    {
        uiState->selectedWasp = MouseRayCast::selectWasp(x, y);      
    }
}