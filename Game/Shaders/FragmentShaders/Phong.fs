#version 330 core

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


in vec3 normal;
in vec3 position;
in vec2 uv0;

out vec4 Fragcolor;

layout(location=0) uniform Material material;
//uniform vec3 view_pos;

vec4 get_diffuse_color()
{
	return texture(material.diffuse_texture, uv0)*material.diffuse_color;
}

vec4 get_specular_color()
{
	return texture(material.specular_texture, uv0)*material.specular_color;
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
	vec3 normal = normalize(normal);
	vec3 lightDir = normalize(lightPos - position);

	float diffuse = lambert(lightDir, normal);
	float specular = 0.0;
	if(diffuse> 0.0 && k_specular && shininess > 0.0)
	{
		vec3 viewPos = transpose(mat3(view))*(-view[3].xyz);
		specular = specular_phong(lightDir, position, normal, viewPos, shininess);
	}

	vec3 emissive_color = texture2D(texture3, uv0).rgb;
	vec3 occlusion_color= texture2D(texture2, uv0).rgb;
	vec4 specular_color = texture2D(texture1, uv0);
	vec4 diffuse_color = texture2D(texture0, uv0);

	vec3 color = emissive_color + //emissive
				 diffuse_color.rgb * occlusion_color * k_ambient + //ambient
				 diffuse_color.rgb * diffuse * k_diffuse + //diffuse
				 specular_color.rgb * specular * k_specular + //specular
				 
	Fragcolor = vec4(color, diffuse_color.a);

	/*float intensity = k_ambient * ambient + k_diffuse*diffuse + k_specular*specular;
	
	if(readTexture)
	{
		Fragcolor = texture2D(texture0, uv0)*intensity*Vcolor;
	}else
	{
		Fragcolor = intensity*Vcolor;
	}*/
}
