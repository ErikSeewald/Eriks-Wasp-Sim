/**
 * @file WaspRenderer.cpp
 * @brief This file specifies the implementation of WaspRenderer.h
 */

#include "WaspRenderer.h"
#include <GL/glew.h>
#include <DirectXMath.h>
#include <iostream>
#include <cstdlib>
#include "SimVisualizer.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

using DirectX::XMFLOAT3;
using WaspRenderer::Vertex;

//MESH
GLuint VBO, VAO, EBO;
std::vector<unsigned int> vertexIndices;

//FILE
const std::string baseDir = "../../../../../Models/wasp/";
const std::string baseDirFallback = "../../../Models/wasp/";
const std::string modelFile = baseDir + "Wasp.obj";
const std::string modelFileFallback = baseDirFallback + "Wasp.obj";

/**
* Visualizes the given list of wasps.
*
* @param wasp the std::list of wasps to visualize
*/
void WaspRenderer::drawWasps(std::list<Wasp*>* wasps)
{
    static float pi = 3.14159265359f;

    glColor3f(1.0f, 0.5f, 0.0f);
    glBindVertexArray(VAO);
    for (Wasp* wasp : *wasps)
    {
        XMFLOAT3 position = wasp->getPosition();
        XMFLOAT3 viewingVector = wasp->getViewingVector();

        glPushMatrix();
        glTranslatef(position.x, position.y, position.z);

        // Rotate around y-axis to match viewing vector
        float angle = atan2(viewingVector.x, viewingVector.z);
        glRotatef(angle * (180.0f / pi), 0.0f, 1.0f, 0.0f);

        glDrawElements(GL_TRIANGLES, vertexIndices.size(), GL_UNSIGNED_INT, 0);

        glPopMatrix();
    }
    glBindVertexArray(0);
}

/**
* Initializes the WaspRenderer.
*/
void WaspRenderer::init()
{
    _loadModel();
}

/**
* Loads the wasp model and binds it to VBO, VAO and EBO.
*/
void WaspRenderer::_loadModel()
{
    //LOAD DATA
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes; // in this case the obj only holds one shape -> wasp mesh
    std::vector<tinyobj::material_t> materials;
    std::string err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, modelFile.c_str(), baseDir.c_str()))
    {
        //Model file not found -> try modelFileFallback
        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, modelFileFallback.c_str(), baseDirFallback.c_str()))
        {
            std::cerr << err << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    //CONVERT DATA
    std::vector<Vertex> vertices;
    for (const tinyobj::shape_t shape : shapes) 
    {
        for (const tinyobj::index_t index : shape.mesh.indices) 
        {
            Vertex vertex{};

            //POSITION
            vertex.position[0] = attrib.vertices[3 * index.vertex_index + 0];
            vertex.position[1] = attrib.vertices[3 * index.vertex_index + 1];
            vertex.position[2] = attrib.vertices[3 * index.vertex_index + 2];

            //NORMALS
            if (index.normal_index >= 0) 
            {
                vertex.normal[0] = attrib.normals[3 * index.normal_index + 0];
                vertex.normal[1] = attrib.normals[3 * index.normal_index + 1];
                vertex.normal[2] = attrib.normals[3 * index.normal_index + 2];
            }

            //TEXTURE COORDINATES
            if (index.texcoord_index >= 0) 
            {
                vertex.texcoord[0] = attrib.texcoords[2 * index.texcoord_index + 0];
                vertex.texcoord[1] = attrib.texcoords[2 * index.texcoord_index + 1];
            }

            vertices.push_back(vertex);
            vertexIndices.push_back(vertexIndices.size());
        }
    }
    
    //BIND TO GL
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexIndices.size() * sizeof(unsigned int), vertexIndices.data(), GL_STATIC_DRAW);

    //POSITION
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    //NORMALS
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //TEXTURE COORDINATES
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0);
}