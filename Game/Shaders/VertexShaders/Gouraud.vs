#version 330
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal; //TODO: change texcoords and normal location
layout(location = 2) in vec2 uv0;

struct Material
{
    sampler2D diffuse_texture;
    vec4      diffuse_color;

    sampler2D specular_texture;
    vec3      specular_color;
    float     shininess;

    sampler2D occlusion_texture;

    sampler2D emissive_texture;
    vec3      emissive_color;

    float     k_ambient;
    float     k_diffuse;
    float     k_specular;
};

layout (std140) uniform Matrices
{
    mat4 proj;
    mat4 view;
};

uniform mat4 model;
uniform Material material;
uniform vec3 lightPos;

out vec4 fcolor;

vec4 get_diffuse_color()
{
	return texture(material.diffuse_texture, uv0)*material.diffuse_color;
}

vec3 get_specular_color()
{
	return texture(material.specular_texture, uv0).rgb*material.specular_color;
}

vec3 get_occlusion_color()
{
	return texture(material.occlusion_texture, uv0).rgb;
}

vec3 get_emissive_color()
{
	return texture(material.emissive_texture, uv0).rgb*material.emissive_color;
}

float lambert(vec3 direction, vec3 normals)
{
	return max(dot(normals, direction), 0.0);
}

float specular_phong(vec3 direction, vec3 pos, vec3 normals, vec3 viewPos, float shininess)
{
	vec3 viewDir = normalize(viewPos-pos);
	vec3 reflectDir = normalize(reflect(-direction, normals));
	return pow(max(dot(viewDir, reflectDir), 0.0), shininess);
}

void main()
{
	vec3 position = (model * vec4(vertex_position, 1.0)).xyz;
	vec3 normal = (model * vec4(vertex_normal, 0.0)).xyz; 
	gl_Position = proj*view*vec4(position, 1.0);

	normal = normalize(normal);
	vec3 lightDir = normalize(lightPos - position);

	float diffuse = lambert(lightDir, normal);
	float specular = 0.0;
	if(diffuse> 0.0 && material.k_specular>0.0 && material.shininess > 0.0)
	{
		vec3 viewPos = transpose(mat3(view))*(-view[3].xyz);
		specular = specular_phong(lightDir, position, normal, viewPos, material.shininess);
	}

	vec3 emissive_color = get_emissive_color();
	vec3 occlusion_color= get_occlusion_color();
	vec3 specular_color = get_specular_color();
	vec4 diffuse_color = get_diffuse_color();

	vec3 color = emissive_color + //emissive
				 diffuse_color.rgb * occlusion_color * material.k_ambient + //ambient
				 diffuse_color.rgb * diffuse * material.k_diffuse + //diffuse
				 specular_color.rgb * specular * material.k_specular; //specular
				 
	fcolor = vec4(color, diffuse_color.a);
}