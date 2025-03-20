#version 330 core

// Vertex Attribute Data
layout (location = 0) in vec3 aPos;		// Vertex position in object space

// Instance Data
layout (location = 2) in vec3 iPosition;
layout (location = 3) in vec4 iColor;

uniform mat4 uViewProj;

out vec4 fragColor;

// Vert shader for InstanceDataBasic. Only takes an instance position and an instance color.
// The latter is passed on to the frag shader.
void main()
{
	gl_Position = uViewProj * vec4(aPos + iPosition, 1.0);
	fragColor = iColor;
}