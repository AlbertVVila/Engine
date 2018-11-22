#version 330 core
out vec4 Fragcolor;

uniform vec4 Vcolor;
in vec3 normal;

void main()
{
	vec3 normal = normalize(normal);
	Fragcolor= Vcolor;
}
