// shaders/fragment.glsl
#version 330 core
out vec4 FragColor;

uniform vec3 lineColor; // Color passed from the application

void main()
{
    FragColor = vec4(lineColor, 1.0);
}