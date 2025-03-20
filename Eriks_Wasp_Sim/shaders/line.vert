#version 330 core

// Vertex Attribute Data
layout (location = 0) in vec3 aPos;      // Vertex position in object space
layout (location = 1) in vec3 aNormal;   // Vertex normal

// Instance Data
layout (location = 2) in vec3 iStart;
layout (location = 3) in vec3 iEnd;
layout (location = 4) in vec4 iColor;

uniform mat4 uViewProj;

out vec4 fragColor;

// Vert shader for drawing a instanced line between two points (iStart, iEnd).
// Computes the transformation of the original line model (in Assets/) to match the intended line.
// Passes the instance color to the frag shader.
void main()
{
    vec3 lineVec = iEnd - iStart;
    float lengthLine = length(lineVec);

    // Normalize
    vec3 vecNormalized = normalize(lineVec);

    // Rotation
    vec3 upVector = vec3(0.0, 1.0, 0.0);
    float dotProd = dot(upVector, vecNormalized);
    float angle = -acos(dotProd);

    // Rotation matrix using Rodrigues' formula
    mat3 rotationMatrix = mat3(1.0);
    if (abs(dotProd - 1.0) > 0.0001 && abs(dotProd + 1.0) > 0.0001)
    {
        vec3 axis = normalize(cross(upVector, vecNormalized));

        float cosA = cos(angle);
        float sinA = sin(angle);

        mat3 R = mat3(
            0, -axis.z, axis.y,
            axis.z, 0, -axis.x,
            -axis.y, axis.x, 0
        );

        rotationMatrix = mat3(1.0) + sinA * R + (1.0 - cosA) * (R * R);
    }

    // Scaling along Y-axis (Because line model verts are 0,0,0 to 0,1,0)
    vec3 transformedPos = rotationMatrix * (aPos * vec3(1.0, lengthLine, 1.0));

    // Translation
    transformedPos += iStart;

	// Final vertex position
    gl_Position = uViewProj * vec4(transformedPos, 1.0);
    
    fragColor = iColor;
}
