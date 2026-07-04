#version 330 core

flat in int w_IndexFlat;
flat in uint waspBitmapFlat; // See WaspRenderer.ccp for the bitmap format
flat in float relativeWorkerScoreFlat;
flat in float relativeHungerFlat;
flat in float relativeHealthFlat;
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

    vec3 baseColor;
    uint renderMode = waspBitmapFlat >> 28;
    switch (renderMode)
    {
        case 0u: // UniformFlat
            baseColor = vec3(1.0, 0.5, 0.0); 
            brightness = 1.0; 
            break;

        case 1u: // UniformColor
            baseColor = vec3(1.0, 0.5, 0.0); 
            break; 

        case 2u: // RandomOranges                                          
            float h = 1.0 - (hash(w_IndexFlat) * 0.6);
            baseColor = vec3(1.0 * h, 0.5 * h, 0.1);
            break;
        
        case 3u: // FullRandomColors
            baseColor = vec3(hash(w_IndexFlat), hash(w_IndexFlat + 1), hash(w_IndexFlat + 2)); 
            break;

        case 4u: // GreenIfHasGoal
            bool hasGoal = (waspBitmapFlat & 2u) == 2u;
            baseColor = hasGoal ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);
            break;     

        case 5u: // RelativeWorkerScore
            bool isQueen = (waspBitmapFlat & 1u) == 1u;
            float r = relativeWorkerScoreFlat;
            baseColor = isQueen ? vec3(0.0, 1.0, 0.0) : vec3(r, 0.1, 1.0 - r);
            break;

        case 6u: // RelativeHunger
            baseColor = vec3(1.0 - relativeHungerFlat, relativeHungerFlat, 0.1);
            break;

        case 7u: // RelativeHealth
            baseColor = vec3(1.0 - relativeHealthFlat, relativeHealthFlat, 0.1);
            break; 

        case 8u: // IsContractPartner
            bool isPartner = (waspBitmapFlat & 4u) == 4u;
            float hasContract = (waspBitmapFlat & 8u) == 8u ? 1.0 : 0.25;
            baseColor = isPartner ? vec3(0.0, hasContract, 0.0) : vec3(hasContract, 0.0, 0.0);
            break;
    }
    
    vec3 color = baseColor * brightness;
    FragColor = vec4(color, 1.0);
}