#version 330
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal; //TODO: change texcoords and normal location
layout(location = 2) in vec2 vertex_uv0;

layout (std140) uniform Matrices
{
    mat4 proj;
    mat4 view;
};

uniform mat4 model;

uniform vec3  lightPos;
uniform float ambient;
uniform float shininess;
uniform float k_ambient;
uniform float k_diffuse;
uniform float k_specular;

flat out float intensity;

void main()
{
	vec3 position = (model * vec4(vertex_position, 1.0)).xyz;
	vec3 normal = normalize((model * vec4(vertex_normal, 0.0)).xyz); //Avoid translation
	gl_Position = proj*view*vec4(position, 1.0);

	vec3 lightDir = normalize(lightPos - position);
	float diffuse = max(dot(normal, lightDir), 0.0);
	//vec3 diffuse = diff * lightColor;
	float specular = 0.0;

	if(diffuse > 0.0 && k_specular > 0.0 && shininess > 0.0)
	{
		vec3 viewPos = transpose(mat3(view))*(-view[3].xyz); //wat
		vec3 viewDir = normalize(viewPos-position);
		vec3 reflectDir = reflect(-lightDir, normal);
		float specular = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
	}

	intensity = k_ambient * ambient + k_diffuse*diffuse + k_specular*specular;
}