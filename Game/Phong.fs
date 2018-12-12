#version 330 core

layout (std140) uniform Matrices
{
    mat4 proj;
    mat4 view;
};

out vec4 Fragcolor;
uniform vec4 Vcolor;

uniform vec3  lightPos;
uniform float ambient;
uniform float shininess;
uniform float k_ambient;
uniform float k_diffuse;
uniform float k_specular;

in vec3 normal;
in vec3 position;

void main()
{
	vec3 normal = normalize(normal);
	vec3 lightDir = normalize(lightPos - position);
	float diffuse = max(dot(normal, lightDir), 0.0);
	//vec3 diffuse = diff * lightColor;
	float specular = 0.0;

	if(diffuse > 0.0 && k_specular > 0.0 && shininess > 0.0)
	{
		vec3 viewPos = transpose(mat3(view))*(-view[3].xyz);
		vec3 viewDir = normalize(viewPos-position);
		vec3 reflectDir = normalize(reflect(-lightDir, normal));
		specular = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
	}

	float intensity = k_ambient * ambient + k_diffuse*diffuse + k_specular*specular;
	Fragcolor = intensity*Vcolor;
}
