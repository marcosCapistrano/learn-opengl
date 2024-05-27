#version 330 core

in vec4 VertexPos;
out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f); // Red intensity based on distance
}