#ifndef __ResourceMaterial_h__
#define __ResourceMaterial_h__

#include "Resource.h"
#include "Math/float4.h"
#include <list>

#define MAXTEXTURES 4
#define DEFAULTMAT "Default"

enum class TextureType
{
	DIFFUSE = 0,
	SPECULAR,
	OCCLUSION,
	EMISSIVE
};

class ResourceTexture;
struct Shader;

class ResourceMaterial :
	public Resource
{
public:
	ResourceMaterial();
	~ResourceMaterial();

	//bool LoadInMemory() override;
	//void DeleteFromMemory() override;
	//void Save(JSON_value &config) const override;
	//void Load(const JSON_value &config) override;

public:
	std::string name;
	Shader* shader = nullptr;

	ResourceTexture* textures[MAXTEXTURES]{ nullptr };

	float4 diffuse_color = float4::one;
	float3 specular_color = float3::one;
	float3 emissive_color = float3::one;

	float kAmbient = 0.3f;
	float kDiffuse = 0.2f;
	float kSpecular = 0.1f;
	float shininess = 32.f;
};

#endif __ResourceMaterial_h__