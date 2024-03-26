#include "FoodRenderer.h"
#include "SimVisualizer.h"

using Food::FoodEntity;

void debugCubeRemoveLater()
{
    glBegin(GL_QUADS);

    glVertex3f(-0.2f, -0.2f, 0.2f);
    glVertex3f(0.2f, -0.2f, 0.2f);
    glVertex3f(0.2f, 0.2f, 0.2f);
    glVertex3f(-0.2f, 0.2f, 0.2f);

    glVertex3f(-0.2f, -0.2f, -0.2f);
    glVertex3f(-0.2f, 0.2f, -0.2f);
    glVertex3f(0.2f, 0.2f, -0.2f);
    glVertex3f(0.2f, -0.2f, -0.2f);

    glVertex3f(-0.2f, 0.2f, -0.2f);
    glVertex3f(-0.2f, 0.2f, 0.2f);
    glVertex3f(0.2f, 0.2f, 0.2f);
    glVertex3f(0.2f, 0.2f, -0.2f);

    glVertex3f(-0.2f, -0.2f, -0.2f);
    glVertex3f(0.2f, -0.2f, -0.2f);
    glVertex3f(0.2f, -0.2f, 0.2f);
    glVertex3f(-0.2f, -0.2f, 0.2f);

    glVertex3f(0.2f, -0.2f, -0.2f);
    glVertex3f(0.2f, 0.2f, -0.2f);
    glVertex3f(0.2f, 0.2f, 0.2f);
    glVertex3f(0.2f, -0.2f, 0.2f);

    glVertex3f(-0.2f, -0.2f, -0.2f);
    glVertex3f(-0.2f, -0.2f, 0.2f);
    glVertex3f(-0.2f, 0.2f, 0.2f);
    glVertex3f(-0.2f, 0.2f, -0.2f);

    glEnd();
}

/**
* Visualizes the given FoodSlot linked list. Assumes glut, glew, etc. are preinitialized.
*/
void FoodRenderer::drawFoodSlots(EntitySlot* foodSlot)
{
    FoodEntity* food{};

    glColor3f(0.4f, 0.1f, 0.05f);
    while (foodSlot != nullptr)
    {
        food = (FoodEntity*) foodSlot->entity;

        glPushMatrix();
        glm::vec3 position = food->position;
        glTranslatef(position.x, position.y, position.z);

        debugCubeRemoveLater();

        glPopMatrix();

        foodSlot = foodSlot->next;
    }
}