#include "WaspRenderer.h"
#include <iostream>
#include "SimVisualizer.h"
#include "ModelHandler.h"
#include "UI.h"

//MESH
GLuint wasp_VAO;
int wasp_vertexCount;

//FILE
const std::string baseDir = "../../../../../Assets/Models/wasp/";
const std::string baseDirFallback = "../../../Assets/Models/wasp/";
const std::string modelFile = baseDir + "Wasp.obj";
const std::string modelFileFallback = baseDirFallback + "Wasp.obj";

/**
* Initializes the WaspRenderer.
*/
void WaspRenderer::init()
{
    if (!ModelHandler::loadModel(baseDir, modelFile, &wasp_VAO, &wasp_vertexCount))
    {
        // FALLBACK
        if (!ModelHandler::loadModel(baseDirFallback, modelFileFallback, &wasp_VAO, &wasp_vertexCount))
        {
            std::cerr << "Failed to load wasp model" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
}

/**
* Visualizes the given WaspSlot linked list. Assumes glut, glew, etc. are preinitialized.
*/
void WaspRenderer::drawWasps(EntitySlot* waspSlot)
{
    Wasp* selectedWasp = UI::getUIState()->selectedWasp;
    Wasp* wasp;

    glColor3f(1.0f, 0.5f, 0.0f);
    glBindVertexArray(wasp_VAO);
    while (waspSlot != nullptr)
    {
        wasp = (Wasp*) waspSlot->entity;
        waspSlot = waspSlot->next;

        if (wasp == selectedWasp)
        {
            continue;
        }

        _drawWaspPrebound(wasp);
    }
    glBindVertexArray(0);
}

/**
* Visualizes the wasp selected by the user, provided it exists.
*/
void WaspRenderer::drawSelectedWasp() 
{
    UI::UI_STATE* uiState = UI::getUIState();

    //GET SELECTED WASP
    Wasp* wasp = uiState->selectedWasp;
    if (wasp == NULL)
    {
        return;
    }

    glColor3f(1.0f, 0.0f, 0.0f);
    glBindVertexArray(wasp_VAO);
    _drawWaspPrebound(wasp);
    glBindVertexArray(0);

    //GOAL
    if (uiState->drawSelectedWaspGoal && wasp->getCurrentGoal() != nullptr)
    {
        glm::vec3 goal = *wasp->getCurrentGoal();
        glm::vec3 position = wasp->getPosition();

        glColor3f(0.0f, 1.0f, 1.0f);
        glBegin(GL_LINES);
        glVertex3f(position.x, position.y, position.z);
        glVertex3f(goal.x, goal.y, goal.z);
        glEnd();
    }
}

/**
* Draws the given wasp with the assumption that the correct vertex array has already been bound and the
* color has been set. This allows for the function to only contain code unique to each wasp being drawn in
* drawWasps()'s loop and thereby increases performance.
*
* @param wasp the wasp to draw
*/
void WaspRenderer::_drawWaspPrebound(Wasp* wasp)
{
    glm::vec3 position = wasp->getPosition();
    glm::vec3 viewingVector = wasp->getViewingVector();

    glPushMatrix();
    glTranslatef(position.x, position.y, position.z);

    // Rotate around y-axis to match viewing vector
    float angle = atan2(viewingVector.x, viewingVector.z);
    glRotatef(SimVisualizer::radToDeg(angle), 0.0f, 1.0f, 0.0f);

    glDrawElements(GL_TRIANGLES, wasp_vertexCount, GL_UNSIGNED_INT, 0);

    glPopMatrix();
}