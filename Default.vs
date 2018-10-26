#version 330
layout(location = 0) in vec3 vertex_position;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

out vec4 vertexColor;

void main()
{
 gl_Position = proj*view*model*vec4(vertex_position, 1.0);
 vertexColor = vec4(1.0, 1.0, 1.0, 1.0);
}