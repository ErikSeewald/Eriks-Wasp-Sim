#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

layout (location = 3) in mat4 aInstanceMatrix;

uniform mat4 uViewProj;

out vec3 vNormal;

void main()
{
    gl_Position = uViewProj * aInstanceMatrix * vec4(aPos, 1.0);
    vNormal = mat3(aInstanceMatrix) * aNormal; 
}