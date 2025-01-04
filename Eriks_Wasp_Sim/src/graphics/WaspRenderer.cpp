#include "WaspRenderer.h"
#include <iostream>
#include "SimVisualizer.h"
#include "ModelHandler.h"
#include "ShaderHandler.h"
#include "UI.h"


//MESH
GLuint wasp_VAO;
GLuint wasp_VBO;
GLuint wasp_EBO;
GLuint wasp_instanceVBO;
int wasp_vertexCount;

//SHADER
GLuint waspShaderProgram = 0;
const std::string shaderDir = "../../../../shaders";
const std::string shaderDirFallback = "../../shaders";

//MODEL FILE
const std::string baseDir = "../../../../../Assets/Models/wasp/";
const std::string baseDirFallback = "../../../Assets/Models/wasp/";
const std::string modelFile = baseDir + "Wasp.obj";
const std::string modelFileFallback = baseDirFallback + "Wasp.obj";

/**
* Initializes the WaspRenderer.
*/
void WaspRenderer::init()
{
    if (!ModelHandler::loadModel(baseDir, modelFile, &wasp_VAO, &wasp_VBO, &wasp_EBO, &wasp_vertexCount))
    {
        // FALLBACK
        if (!ModelHandler::loadModel(baseDirFallback, modelFileFallback, &wasp_VAO, &wasp_VBO, &wasp_EBO, &wasp_vertexCount))
        {
            std::cerr << "Failed to load wasp model" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    // CREATE INSTANCE VBO FOR HARDWARE INSTANCING
    glBindVertexArray(wasp_VAO);
    glGenBuffers(1, &wasp_instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, wasp_instanceVBO);

    // Locations 3,4,5,6 for the matrix columns
    for (int i = 0; i < 4; i++)
    {
        glEnableVertexAttribArray(3 + i);
        glVertexAttribPointer(3 + i,
            4, GL_FLOAT, GL_FALSE,
            sizeof(glm::mat4),
            (void*)(sizeof(glm::vec4) * i));
        glVertexAttribDivisor(3 + i, 1);
    }

    glBindVertexArray(0);

    // LOAD, COMPILE AND LINK SHADERS
    try
    {
        std::string vertSource, fragSource;
        try 
        {
            vertSource = ShaderHandler::loadShaderFile(shaderDir + "/wasp.vert");
            fragSource = ShaderHandler::loadShaderFile(shaderDir + "/wasp.frag");
        }
        catch (std::runtime_error) // FALLBACK PATHS
        {
            vertSource = ShaderHandler::loadShaderFile(shaderDirFallback + "/wasp.vert");
            fragSource = ShaderHandler::loadShaderFile(shaderDirFallback + "/wasp.frag");
        }

        GLuint vs = ShaderHandler::compileShader(GL_VERTEX_SHADER, vertSource);
        GLuint fs = ShaderHandler::compileShader(GL_FRAGMENT_SHADER, fragSource);

        waspShaderProgram = ShaderHandler::linkShaderProgram(vs, fs);

        // CLEAN UP
        glDeleteShader(vs);
        glDeleteShader(fs);
    }

    catch (const std::exception& e)
    {
        std::cerr << "Shader initialization error: " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}

/**
* Visualizes the given WaspSlot linked list. Assumes glut, glew, etc. are preinitialized.
*/
void WaspRenderer::drawWasps(EntitySlot* waspSlot)
{
    // COLLECT INSTANCE DATA
    std::vector<glm::mat4> instanceData;
    instanceData.reserve(WaspSlots::MAX_WASP_COUNT);

    Wasp* selectedWasp = UI::getUIState()->selectedWasp;
    EntitySlot* currentSlot = waspSlot;
    while (currentSlot != nullptr)
    {
        Wasp* wasp = (Wasp*) currentSlot->entity;
        currentSlot = currentSlot->next;

        if (wasp == selectedWasp) { continue; }

        // Optimization: Instead of using atan2(), cos() and sin() which cause too much overhead
        // IMPORTANT: This only works based on the assumption that the components x and z are always normalized
        // The y component is allowed to change (which means the 3d vector does not need to be normalized)
        float cos = wasp->viewingVector.z;
        float sin = wasp->viewingVector.x;

        // Optimization: Manually construct model matrix instead of using transform and rotate functions
        // to skip any unnecessary steps.
        glm::mat4 model = glm::mat4(1.0f);

        // Translate
        model[3][0] = wasp->position.x;
        model[3][1] = wasp->position.y;
        model[3][2] = wasp->position.z;

        // Rotate around y axis
        model[0][0] = cos;
        model[0][1] = 0.0f;
        model[0][2] = -sin;
        model[1][0] = 0.0f;
        model[1][1] = 1.0f;
        model[1][2] = 0.0f;
        model[2][0] = sin;
        model[2][1] = 0.0f;
        model[2][2] = cos;
        
        instanceData.push_back(model);
    }

    // DRAW
    ShaderHandler::drawInstanceData(&instanceData, &wasp_VAO, &wasp_instanceVBO, wasp_vertexCount, &waspShaderProgram);
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
    glm::vec3 position = wasp->position;
    glm::vec3 viewingVector = wasp->viewingVector;

    glPushMatrix();
    glTranslatef(position.x, position.y, position.z);

    // Rotate around y-axis to match viewing vector
    float angle = atan2(viewingVector.x, viewingVector.z);
    glRotatef(SimVisualizer::radToDeg(angle), 0.0f, 1.0f, 0.0f);

    glDrawElements(GL_TRIANGLES, wasp_vertexCount, GL_UNSIGNED_INT, 0);

    glPopMatrix();
    glBindVertexArray(0);

    //GOAL
    if (uiState->drawSelectedWaspGoal && wasp->getCurrentGoal() != nullptr)
    {
        glm::vec3 goal = *wasp->getCurrentGoal();
        glm::vec3 position = wasp->position;

        glColor3f(0.0f, 1.0f, 1.0f);
        glBegin(GL_LINES);
        glVertex3f(position.x, position.y, position.z);
        glVertex3f(goal.x, goal.y, goal.z);
        glEnd();
    }
}