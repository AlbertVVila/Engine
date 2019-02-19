#version 330
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 vertex_uv0;
layout(location = 3) in vec3 vertex_tangent;

layout (std140) uniform Matrices
{
    mat4 proj;
    mat4 view;
};

uniform mat4 model;
out vec3 normalIn;
out vec3 position;
out vec2 uv0;
out mat3 TBNMat;
out vec3 viewPos;

void main()
{
	position = (model * vec4(vertex_position, 1.0)).xyz;
	gl_Position = proj*view*vec4(position, 1.0);

	normalIn = mat3(model) * vertex_normal;
	vec3 tan = mat3(model) * vertex_tangent;
	vec3 bitan = cross(vertex_tangent, vertex_normal);
	
	vec3 N = normalize(normalIn);
	vec3 T = normalize(tan);
	vec3 B = cross(T, normalIn);	
	TBNMat = transpose(mat3(T, B, N));
	position = TBNMat * position;
	viewPos = TBNMat * transpose(mat3(view))*(-view[3].xyz);	
	uv0 = vertex_uv0;
}