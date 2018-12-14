#version 330 core
out vec4 Fragcolor;
uniform vec4 Vcolor;

flat in float intensity;

void main()
{
	Fragcolor = intensity*Vcolor;
}