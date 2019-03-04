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
	ResourceMaterial(unsigned uid);
	ResourceMaterial(const ResourceMaterial& resource);
	~ResourceMaterial();

	//bool LoadInMemory() override;
	void DeleteFromMemory() override;
	void Save() const;
	void Load(const char* materialfile);
	void Reset(const ResourceMaterial& material);

	ResourceTexture* GetTexture(TextureType type) const;
	std::list<ResourceTexture*> GetTextures() const;
	void SetUniforms(unsigned shader) const;

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