#include "MouseHandler.h"

#include <GL/glut.h>
#include "UI.h"
#include "MouseRayCast.h"
#include "imgui.h"

// Get a reference to the ImGui MouseFunc so we can call it from our own MouseHandler
extern void ImGui_ImplGLUT_MouseFunc(int button, int state, int x, int y);

void MouseHandler::mouseClick(int button, int state, int x, int y)
{
    // Call the imgui MouseFunc. If the mouse event occurred inside the gui, we return afterwards.
    // If not, then we move on to our own mouse handling functions.
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