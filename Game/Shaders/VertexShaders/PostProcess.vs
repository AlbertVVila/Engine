layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec2 vertex_uv0;

out vec2 UV0;

uniform mat4 model;

void main()
{
   	UV0 = vertex_uv0;
	gl_Position =  model * vec4(vertex_position, 1.0);
}