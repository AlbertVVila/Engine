#version 330
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec4 vertex_color;

layout (std140) uniform Matrices
{
    mat4 proj;
    mat4 view;
};

uniform mat4 model;
out vec4 fcolor;

void main()
{
	fcolor = vertex_color;
	gl_Position = proj*view*model*vec4(vertex_position, 1.0);
}