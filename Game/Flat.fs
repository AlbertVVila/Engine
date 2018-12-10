#version 330 core
out vec4 Fragcolor;
uniform vec4 Vcolor;
uniform float ambient;
in vec2 uv0;

void main()
{
 Fragcolor= ambient*Vcolor;
}
