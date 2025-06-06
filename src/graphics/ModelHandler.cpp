#include "ModelHandler.h"
#include "DirectoryHandler.h"
#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

using ModelHandler::Vertex;

// DIRECTORIES
const std::string baseDir = "assets/models/";

/**
* Loads the model corresponding to the fileName, binds it to VAO, VBO, EBO and writes
* the vertex count of the model into vertexCount.
*
* @param fileName The path and file name relative to the 'assets/models/' directory (e.g. 'modelName/modelFile.obj')
* @return bool - success of the operation
*/
bool ModelHandler::loadModel(const std::string& fileName, GLuint* VAO, GLuint* VBO, GLuint* EBO, int* vertexCount)
{
    //LOAD DATA
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes; // in this case the obj only holds one shape -> wasp mesh
    std::vector<tinyobj::material_t> materials;
    std::string err;

    std::string baseDirPath = DirectoryHandler::appendToProjectRoot(baseDir);
    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, (baseDirPath +fileName).c_str(), baseDirPath.c_str()))
    {
        return false;
    }

    //CONVERT DATA
    std::vector<Vertex> vertices;
    std::vector<unsigned int> vertexIndices{};
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
    
    *vertexCount = vertexIndices.size();

    //BIND TO GL
    glGenVertexArrays(1, VAO);
    glBindVertexArray(*VAO);

    glGenBuffers(1, VBO);
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
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

    return true;
}