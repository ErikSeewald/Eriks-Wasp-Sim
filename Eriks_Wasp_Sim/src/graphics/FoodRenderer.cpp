#include "FoodRenderer.h"
#include "SimVisualizer.h"
#include "ModelHandler.h"
#include "ShaderHandler.h"
#include <vector>
#include <iostream>

// THIS IS THE NEW VERSION

using Food::FoodEntity;

//MESH
GLuint food_VAO;
GLuint food_VBO;
GLuint food_EBO;
GLuint food_instanceVBO;
int food_vertexCount;

//SHADER
GLuint foodShaderProgram = 0;
const std::string shaderDir = "../../../../shaders";
const std::string shaderDirFallback = "../../shaders";

//FILE
const std::string baseDir = "../../../../../Assets/Models/food/";
const std::string baseDirFallback = "../../../Assets/Models/food/";
const std::string modelFile = baseDir + "Food.obj";
const std::string modelFileFallback = baseDirFallback + "Food.obj";

/**
* Initializes the FoodRenderer.
*/
void FoodRenderer::init()
{
    if (!ModelHandler::loadModel(baseDir, modelFile, &food_VAO, &food_VBO, &food_EBO, &food_vertexCount))
    {
        // FALLBACK
        if (!ModelHandler::loadModel(baseDirFallback, modelFileFallback, &food_VAO, &food_VBO, &food_EBO, &food_vertexCount))
        {
            std::cerr << "Failed to load food model" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    // CREATE INSTANCE VBO FOR HARDWARE INSTANCING
    glBindVertexArray(food_VAO);
    glGenBuffers(1, &food_instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, food_instanceVBO);

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
            vertSource = ShaderHandler::loadShaderFile(shaderDir + "/food.vert");
            fragSource = ShaderHandler::loadShaderFile(shaderDir + "/food.frag");
        }
        catch (std::runtime_error) // FALLBACK PATHS
        {
            vertSource = ShaderHandler::loadShaderFile(shaderDirFallback + "/food.vert");
            fragSource = ShaderHandler::loadShaderFile(shaderDirFallback + "/food.frag");
        }

        GLuint vs = ShaderHandler::compileShader(GL_VERTEX_SHADER, vertSource);
        GLuint fs = ShaderHandler::compileShader(GL_FRAGMENT_SHADER, fragSource);

        foodShaderProgram = ShaderHandler::linkShaderProgram(vs, fs);

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
* Visualizes the given FoodSlot linked list. Assumes glut, glew, etc. are preinitialized.
*/
void FoodRenderer::drawFood(EntitySlot* foodSlot)
{
    // COLLECT INSTANCE DATA
    std::vector<glm::mat4> instanceData;
    instanceData.reserve(Food::MAX_FOOD_COUNT);

    EntitySlot* currentSlot = foodSlot;
    while (currentSlot != nullptr)
    {
        FoodEntity* food = (FoodEntity*) currentSlot->entity;
        currentSlot = currentSlot->next;

        // Optimization: Manually construct model matrix instead of using transform and rotate functions
        // to skip any unnecessary steps.
        glm::mat4 model = glm::mat4(1.0f);
        model[3][0] = food->position.x;
        model[3][1] = food->position.y;
        model[3][2] = food->position.z;

        instanceData.push_back(model);
    }

    // DRAW
    ShaderHandler::drawInstanceData(&instanceData, &food_VAO, &food_instanceVBO, food_vertexCount, &foodShaderProgram);
}