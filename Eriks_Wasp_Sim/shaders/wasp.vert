#version 330 core

// Vertex Attribute Data
layout (location = 0) in vec3 aPos;		// Vertex position in object space
layout (location = 1) in vec3 aNormal;	// Vertex normal

// Instance Data
layout (location = 2) in vec3 iPosition;
layout (location = 3) in vec3 iViewingVector;

uniform mat4 uViewProj;

out vec3 vNormal;

// Vert shader for drawing a instanced wasp entity.
// Computes the transformation of the original wasp model (in Assets/) to match the instance position and viewing vector.
// Passes the normal to the frag shader.
void main()
{
	float sinAngle = iViewingVector.x;
	float cosAngle = iViewingVector.z;

	// Rotate around Y
	vec3 rotatedPos;
	rotatedPos.x = aPos.x * cosAngle + aPos.z * sinAngle;
	rotatedPos.y = aPos.y;
	rotatedPos.z = -aPos.x * sinAngle + aPos.z * cosAngle;

	// Translate
	rotatedPos += iPosition;

	// Final vertex position
	gl_Position = uViewProj * vec4(rotatedPos, 1.0);

	// Rotate normal the same way
	vNormal.x = aNormal.x * cosAngle + aNormal.z * sinAngle;
	vNormal.y = aNormal.y;
	vNormal.z = -aNormal.x * sinAngle + aNormal.z * cosAngle;
}