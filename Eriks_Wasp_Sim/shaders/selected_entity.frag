#version 330 core
out vec4 FragColor;

// Fixed color frag shader for highlighting selected entities.
void main() 
{
    FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
