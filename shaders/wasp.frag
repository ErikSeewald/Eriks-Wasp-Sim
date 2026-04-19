#version 330 core

flat in int w_IndexFlat;
in vec3 vNormal;

out vec4 FragColor;

float hash(int x)
{
    return fract(sin(float(x)) * 45678.9123456);
}

// Frag shader for a wasp entity. S
// Scales the constant base color by the input normal.
void main()
{
    vec3 normal = normalize(vNormal);
    vec3 upDirection = vec3(0.0, 1.0, 0.0);

    // Assign brightness based on dot product with upDirection
    float dotProduct = dot(normal, upDirection);
    float brightness = (dotProduct + 1.0) / 2.0;
    brightness = clamp(brightness * 1.2, 0.2, 1.0);

    // Scale base color by brightness factor and index hash
    //float h = 1.0 - (hash(w_IndexFlat) * 0.75);
    //vec3 baseColor = vec3(1.0 * h, 0.5 * h, 0.0);

    vec3 baseColor = vec3(hash(w_IndexFlat), hash(w_IndexFlat + 1), hash(w_IndexFlat + 2));
    vec3 color = baseColor * brightness;

    FragColor = vec4(color, 1.0);
}