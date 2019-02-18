#version 330 core
#define MAX_POINT_LIGHTS 4
#define MAX_SPOT_LIGHTS 4

const float PI = 3.14159265359f; 

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

    float roughness;
	float metallic;
};

struct DirLight
{
	vec3 direction;
	vec3 color;
};

struct PointLight
{
	vec3  position;
	vec3  color;
	vec3  attenuation; //constant + linear + quadratic
};

struct SpotLight
{
	vec3  position;
	vec3  direction;
	vec3  color;
	vec3  attenuation;
	float inner;
	float outer;
};

struct Lights
{
	vec3        ambient_color; 
	DirLight    directional;
	PointLight  points[MAX_POINT_LIGHTS];
	int         num_points;
	SpotLight   spots[MAX_SPOT_LIGHTS];
	int         num_spots;
};

layout (std140) uniform Matrices
{
    mat4 proj;
    mat4 view;
};

in vec3 normalIn;
in vec3 position;
in vec2 uv0;
in vec3 tan;
in vec3 bitan;

out vec4 Fragcolor;

uniform Material material;
uniform Lights lights;

vec4 get_albedo()
{
	return texture(material.diffuse_texture, uv0)*material.diffuse_color;
}

vec3 get_occlusion_color()
{
	return texture(material.occlusion_texture, uv0).rgb;
}

vec3 get_emissive_color()
{
	return texture(material.emissive_texture, uv0).rgb*material.emissive_color;
}

float get_attenuation(vec3 attenuation, float distance)
{
	return 1/(attenuation[0] + distance * attenuation[1] + distance*distance*attenuation[2]);
}

float D(vec3 H, vec3 N)
{
	float r2 = pow(material.roughness, 2);
	float NdotH = dot(N,H);
	float NdotH2 = pow(NdotH, 2);
	float num = pow(r2, 2);
	float den = NdotH2 * (num - 1.f) + 1.f;
	return num / max(den, 0.001);
}

float GGX(vec3 M, vec3 N)
{
	float NdotM = max(dot(N,M), 0.001);

	float r = (material.roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotM;
    float denom = max(NdotM * (1.0 - k) + k, 0.001f);
	
    return num / denom;
}

float GSmith(vec3 L, vec3 V, vec3 N)
{
	return GGX(L, N) * GGX(V, N);
}

vec3 BRDF(vec3 F, vec3 L, vec3 V, vec3 N, vec3 H)
{
	vec3 num = F * GSmith(L, V, N) * D(H, N);
	float den = 4 * dot(N,L) * dot(N,V);
	return num / max(den, 0.001);
}

vec3 FSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
} 


vec3 CalculateNormal()
{
	vec3 normalM = texture(material.occlusion_texture, uv0).xyz;
	normalM = normalize(normalM * 2.0 - 1.0);
	vec3 N = normalize(normalIn);
	vec3 T = normalize(normalize(tan) - N *dot(N, normalize(tan)));
	vec3 B = cross(T, normalIn);	
	mat3 TBN = mat3(T, B, N);

	return normalize(TBN * normalM);

}



void main()
{
	vec3 normal = CalculateNormal();
	vec3 viewPos = transpose(mat3(view))*(-view[3].xyz);	
	vec4 albedo = get_albedo();
	vec3 emissive_color = get_emissive_color();
	//vec3 occlusion_color= get_occlusion_color();
	vec3 occlusion_color = vec3(1,1,1);	
	vec3 F0 = vec3(.04f);
	F0 = mix(F0, albedo.rgb, material.metallic);
	//vec3 color = directional_phong(normal, viewPos, lights.directional, diffuse_color, specular_color);

	vec3 color = vec3(0, 0, 0);
	vec3 N = normal;
	vec3 V = normalize(viewPos - position);

	for(int i=0; i < lights.num_points; ++i)
	{
		vec3 L = normalize(lights.points[i].position - position);
		vec3 H = normalize(V + L);
		float distance = length(lights.points[i].position - position);
		vec3 radiance = lights.points[i].color * get_attenuation(lights.points[i].attenuation, distance);				
		
		vec3 F = FSchlick(max(dot(H, V), 0.0), F0);   

		vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - material.metallic; //albedo amount		

		float NdotL = max(dot(N, L), 0.0);        
		color += (kD * albedo.rgb / PI + BRDF(F, L, V, N, H)) * radiance * NdotL;  
	}

	for(int i=0; i < lights.num_spots; ++i)
	{
		vec3 L = normalize(lights.spots[i].position - position);
		vec3 H = normalize(V + L);
		float distance = length(lights.spots[i].position - position);

		float theta = dot(normalize(L), normalize(-lights.spots[i].direction));
		float epsilon = max(0.0001, lights.spots[i].inner - lights.spots[i].outer);
		float cone = clamp((theta - lights.spots[i].outer) / epsilon, 0.0, 1.0); 
	
		float att = get_attenuation(lights.spots[i].attenuation, distance) * cone;
		
		vec3 radiance = lights.spots[i].color * att;				
		
		vec3 F = FSchlick(max(dot(H, V), 0.0), F0);   

		vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - material.metallic; //albedo amount		

		float NdotL = max(dot(N, L), 0.0);        
		color += (kD * albedo.rgb / PI + BRDF(F, L, V, N, H)) * radiance * NdotL;
	}
	
	/*color += 	 emissive_color + //emissive
				 diffuse_color.rgb * lights.ambient_color * occlusion_color * material.k_ambient; //ambient
				 */
	Fragcolor = vec4(color, albedo.a);
}
