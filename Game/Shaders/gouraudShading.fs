#version 330 core

uniform vec4 newColor;

out vec4 color;

in float intensity;

void main()
{
    color = vec4(intensity*newColor.x, intensity*newColor.y, intensity*newColor.z, 1.0);
}
