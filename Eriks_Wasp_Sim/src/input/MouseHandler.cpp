#include "MouseHandler.h"

#include <GL/glut.h>
#include "UI.h"
#include "MouseRayCast.h"
#include "imgui.h"

void MouseHandler::mouseClick(int button, int state, int x, int y)
{
    // Call the imgui MouseFunc. If the mouse event occurred inside the gui, return afterwards.
    // If not, then move on to the custom mouse handling functions.
    ImGui_ImplGLUT_MouseFunc(button, state, x, y);
    if (ImGui::GetIO().WantCaptureMouse)
    {
        return;
    }

    UI::UI_STATE* uiState = UI::getUIState();

    //LEFT CLICK
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        uiState->selectedWasp = MouseRayCast::selectWasp(x, y);
    }
}