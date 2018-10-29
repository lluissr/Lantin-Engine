#version 330 core
out vec4 color;
uniform vec4 newColor;
in vec2 uv0;
uniform sampler2D texture0;
void main()
{
 //color = newColor;
 color = texture2D(texture0, uv0);
}
