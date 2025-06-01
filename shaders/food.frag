#version 330 core

in vec3 vNormal;
in vec4 fragColor;

out vec4 FragColor;

// Frag shader for a food entity in the simulation.
// Scales input color by the input normal.
void main()
{
    vec3 normal = normalize(vNormal);
    vec3 upDirection = vec3(0.0, -1.0, 0.0);

    // Assign brightness based on dot product with upDirection
    float dotProduct = dot(normal, upDirection);
    float brightness = (dotProduct + 1.0) / 2.0;
    brightness = clamp(brightness * 1.2, 0.4, 1.0);

    // Scale base color by brightness factor
    FragColor = fragColor * brightness;
}