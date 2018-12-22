#version 330 core
out vec4 Fragcolor;

in vec2 uv0;
uniform sampler2D texture0;

void main()
{
 Fragcolor= texture2D(texture0,uv0);
}
