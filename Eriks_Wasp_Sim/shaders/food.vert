#version 330 core

// Vertex Attribute Data
layout (location = 0) in vec3 aPos;		// Original mesh position
layout (location = 1) in vec3 aNormal;	// Original mesh normal

// Instance Data
layout (location = 2) in vec3 iPosition;
layout (location = 3) in vec4 iColor;

uniform mat4 uViewProj;

out vec3 vNormal;
out vec4 fragColor;

// Vert shader for a food entity. 
// Passes the instance color and normals to the frag shader.
void main()
{
	gl_Position = uViewProj * vec4(aPos + iPosition, 1.0);
	vNormal = aNormal;
	fragColor = iColor;
}