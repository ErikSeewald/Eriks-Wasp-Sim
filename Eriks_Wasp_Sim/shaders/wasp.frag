#version 330 core

in vec3 vNormal;
out vec4 FragColor;

void main()
{
    vec3 normal = normalize(vNormal);
    vec3 upDirection = vec3(0.0, 1.0, 0.0);

    // Assign brightness based on dot product with upDirection
    float dotProduct = dot(normal, upDirection);
    float brightness = (dotProduct + 1.0) / 2.0;
    brightness = clamp(brightness * 1.2, 0.2, 1.0);

    // Scale base color by brightness factor
    vec3 baseColor = vec3(1.0, 0.5, 0.0);
    vec3 color = baseColor * brightness;

    FragColor = vec4(color, 1.0);
}