#version 330 core

in vec4 fragColor;
out vec4 FragColor;

// Basic shader for instancing with a frag color. 
// Relies on the vert shader to pass the instance attribute color to the frag shader.
void main()
{
    FragColor = fragColor;
}