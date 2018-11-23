#version 330
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;

layout (std140) uniform Matrices
{
    mat4 proj;
    mat4 view;
};

uniform mat4 model;
out vec3 normal;

void main()
{
	gl_Position = proj*view*model*vec4(vertex_position, 1.0);
	//normal = model*vec4(vertex_normal, 0.0); //evitar translacion!
}