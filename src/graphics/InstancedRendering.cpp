#include "InstancedRendering.h"
#include "glm/ext.hpp"

using InstancedRendering::InstanceDataBasic;
using InstancedRendering::InstanceDataLine;
using InstancedRendering::InstanceDataWasp;

/**
* Draws the given instance data using hardware instancing with the given modelVAO, instanceVBO,
* vertexCount and shaderProgram. Assumes HardwareInstancing::setupInstancing has been called beforehand.
*/
template <typename T>
void InstancedRendering::drawInstanceData(const std::vector<T>& instanceData, GLuint modelVAO, GLuint instanceVBO,
    int indexCount, GLuint shaderProgram)
{
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        instanceData.size() * sizeof(T),
        instanceData.data(),
        GL_DYNAMIC_DRAW
    );

    // USE SHADER PROGRAM WITH CAMERA TRANSFORM
    const Camera& camera = SimVisualizer::getCamera();
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(
        glGetUniformLocation(shaderProgram, "uViewProj"),
        1,
        GL_FALSE,
        glm::value_ptr(camera.projection * camera.view)
    );

    glBindVertexArray(modelVAO);
    glDrawElementsInstanced(
        GL_TRIANGLES,
        indexCount,
        GL_UNSIGNED_INT,
        (void*)0,
        static_cast<GLsizei>(instanceData.size())
    );

    glUseProgram(0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


//----- IMPLEMENTATIONS -----
template void InstancedRendering::drawInstanceData<InstancedRendering::InstanceDataBasic>(
    const std::vector<InstanceDataBasic>&, GLuint, GLuint, int, GLuint);

template void InstancedRendering::drawInstanceData<InstancedRendering::InstanceDataLine>(
    const std::vector<InstanceDataLine>&, GLuint, GLuint, int, GLuint);

template void InstancedRendering::drawInstanceData<InstancedRendering::InstanceDataWasp>(
    const std::vector<InstanceDataWasp>&, GLuint, GLuint, int, GLuint);

template <>
void InstancedRendering::setupInstancing<InstanceDataBasic>(GLuint VAO, GLuint* instanceVBO)
{
    glBindVertexArray(VAO);
    glGenBuffers(1, instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, *instanceVBO);

    // Position attribute at location 2
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
        2,          // location
        3,          // size (vec3)
        GL_FLOAT,   // data type
        GL_FALSE,   // normalized?
        sizeof(InstanceDataBasic), // Offset between consecutive instances
        (void*) offsetof(InstanceDataBasic, position)
    );
    glVertexAttribDivisor(2, 1);

    // Color attribute at location 3
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(
        3, // location
        4, // size (vec4)
        GL_FLOAT, // data type
        GL_FALSE, // normalized?
        sizeof(InstanceDataBasic), // Offset between consecutive instances
        (void*)offsetof(InstanceDataBasic, color)
    );
    glVertexAttribDivisor(3, 1);

    glBindVertexArray(0);
}

template <>
void InstancedRendering::setupInstancing<InstanceDataLine>(GLuint VAO, GLuint* instanceVBO)
{
    glBindVertexArray(VAO);
    glGenBuffers(1, instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, *instanceVBO);

    // Start and End vects at position 2..3
    for (int i = 0; i < 2; i++)
    {
        glEnableVertexAttribArray(2 + i);
        glVertexAttribPointer(
            2 + i, // location
            3,     // size (vec3)
            GL_FLOAT, // data type
            GL_FALSE, // normalized?
            sizeof(InstanceDataLine), // Offset between consecutive instances
            (void*) (sizeof(glm::vec3) * i) // Offset to element in the instance data struct
        );
        glVertexAttribDivisor(2 + i, 1);
    }

    // Color attribute at location 4
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(
        4, // location
        4, // size (vec4)
        GL_FLOAT, // data type
        GL_FALSE, // normalized?
        sizeof(InstanceDataLine), // Offset between consecutive instances
        (void*)offsetof(InstanceDataLine, color)
    );
    glVertexAttribDivisor(4, 1);

    glBindVertexArray(0);
}

template <>
void InstancedRendering::setupInstancing<InstanceDataWasp>(GLuint VAO, GLuint* instanceVBO)
{
    glBindVertexArray(VAO);
    glGenBuffers(1, instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, *instanceVBO);

    // Position attribute at location 2
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
        2,          // location
        3,          // size (vec3)
        GL_FLOAT,   // data type
        GL_FALSE,   // normalized?
        sizeof(InstanceDataWasp), // Offset between consecutive instances
        (void*) offsetof(InstanceDataWasp, position)
    );
    glVertexAttribDivisor(2, 1);

    // ViewingVector attribute at location 3
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(
        3,          // location
        3,          // size (vec3)
        GL_FLOAT,   // data type
        GL_FALSE,   // normalized?
        sizeof(InstanceDataWasp), // Offset between consecutive instances
        (void*) offsetof(InstanceDataWasp, viewingVector)
    );
    glVertexAttribDivisor(3, 1);

    // w_index attribute at location 4
    glEnableVertexAttribArray(4);
    glVertexAttribIPointer(         // Needs to be IPointer!!
        4,          // location
        1,          // one integer
        GL_INT,     // data type
        sizeof(InstanceDataWasp), // Offset between consecutive instances
        (void*) offsetof(InstanceDataWasp, w_Index)
    );
    glVertexAttribDivisor(4, 1);

    // waspBitmap attribute at location 5
    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(         // Needs to be IPointer!!
        5,          // location
        1,          // one integer
        GL_UNSIGNED_INT,     // data type
        sizeof(InstanceDataWasp), // Offset between consecutive instances
        (void*) offsetof(InstanceDataWasp, waspBitmap)
    );
    glVertexAttribDivisor(5, 1);

    // relativeWorkerScore attribute at location 6
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(
        6,          // location
        1,          // size (float)
        GL_FLOAT,   // data type
        GL_FALSE,   // normalized?
        sizeof(InstanceDataWasp), // Offset between consecutive instances
        (void*) offsetof(InstanceDataWasp, relativeWorkerScore)
    );
    glVertexAttribDivisor(6, 1);

    // relativeHunger attribute at location 7
    glEnableVertexAttribArray(7);
    glVertexAttribPointer(
        7,          // location
        1,          // size (float)
        GL_FLOAT,   // data type
        GL_FALSE,   // normalized?
        sizeof(InstanceDataWasp), // Offset between consecutive instances
        (void*) offsetof(InstanceDataWasp, relativeHunger)
    );
    glVertexAttribDivisor(7, 1);

    // relativeHealth attribute at location 8
    glEnableVertexAttribArray(8);
    glVertexAttribPointer(
        8,          // location
        1,          // size (float)
        GL_FLOAT,   // data type
        GL_FALSE,   // normalized?
        sizeof(InstanceDataWasp), // Offset between consecutive instances
        (void*) offsetof(InstanceDataWasp, relativeHealth)
    );
    glVertexAttribDivisor(8, 1);


    glBindVertexArray(0);
}