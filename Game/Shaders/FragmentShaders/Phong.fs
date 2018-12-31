#version 330 core
#define MAX_POINT_LIGHTS 4
#define MAX_SPOT_LIGHTS 4

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

struct Lights
{
	AmbientLight ambient;
	DirLight     directional;
	PointLight   points[MAX_POINT_LIGHTS];
	uint         num_points;
	SpotLight    spots[MAX_SPOT_LIGHTS];
	uint         num_spots;
}

struct AmbientLight
{
	vec3 color;
}

struct DirLight
{
	vec3 direction;
	vec3 color;
}

struct PointLight
{
	vec3  position;
	vec3  color;
	vec3  attenuation; //constant + linear + quadratic
}

struct SpotLight
{
	vec3  position;
	vec3  direction;
	vec3  color;
	vec3  attenuation;
	float inner;
	float outer;
}

layout (std140) uniform Matrices
{
    mat4 proj;
    mat4 view;
};

in vec3 normal;
in vec3 position;
in vec2 uv0;

out vec4 Fragcolor;

uniform Material material;
uniform Lights lights;

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

vec3 directional_phong(vec3 normal, vec3 viewPos, DirLight dir, vec4 diffuse_color, vec3 specular_color)
{
	float diffuse = lambert(dir.direction, normal);
	float specular = specular_phong(dir.direction, position, normal, viewPos, material.shininess);

	vec3 color = dir.color * (diffuse_color.rgb * diffuse * material.k_diffuse + //diffuse
				 specular_color.rgb * specular * material.k_specular); //specular
	
	return color;
}

vec3 point_phong(vec3 normal, vec3 viewPos, PointLight point, vec4 diffuse_color, vec3 specular_color)
{
	vec3 lightDir = point.position - position;
	float distance = length(ligthDir);
	lightDir = lightDir/distance;
	float att = get_attenuation(point.attenuation, distance);

	float diffuse = lambert(lightDir, normal);
	float specular = specular_phong(lightDir, position, normal, viewPos, shininess);

	return att * point.color * (diffuse_color *(diffuse*material.k_diffuse));
}

vec3 spot_phong(vec3 normal, vec3 viewPos, SpotLight spot, vec4 diffuse_color, vec3 specular_color)
{
	return vec3(0,0,0);
}

void main()
{
	vec3 normal = normalize(normal);
	vec3 viewPos = transpose(mat3(view))*(-view[3].xyz);

	vec4 diffuse_color = get_diffuse_color();
	vec3 specular_color = get_specular_color();
	vec3 emissive_color = get_emissive_color();
	vec3 occlusion_color= get_occlusion_color();

	vec3 color = directional_phong(normal, viewPos, lights.DirLight, diffuse_color, specular_color);

	for(uint i=0; i < lights.num_points; ++i )
	{
		color+= point_phong(normal, viewPos, lights.PointLight[i], diffuse_color, specular_color);
	}

	for(uint i=0; i < lights.num_spots; ++i )
	{
		color+= spot_phong(normal, viewPos, lights.SpotLight[i], diffuse_color, specular_color);
	}
	

	color += 	 emissive_color + //emissive
				 diffuse_color.rgb * occlusion_color * material.k_ambient; //ambient
				 
	Fragcolor = vec4(color, diffuse_color.a);
}
