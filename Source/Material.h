#ifndef __Material_h__
#define __Material_h__

#include "Math/float4.h"
#include <list>

#define MAXTEXTURES 5
#define DEFAULTMAT "Default"

struct Shader;
struct Texture;

enum class TextureType
{
	DIFFUSE = 0,
	SPECULAR,
	OCCLUSION,
	EMISSIVE,
	NORMAL
};

class Material
{
public:
	Material();
	Material(const char* name);
	Material(const Material& material);

	~Material();

	void Load(const char * material);
	void Save() const;
	void Reset(const Material& material);

	Texture * GetTexture(TextureType type) const;
	std::list<Texture*> GetTextures() const;

	void SetUniforms(unsigned shader) const;

public:
	std::string name;
	Shader* shader = nullptr;
	
	Texture* textures[MAXTEXTURES]{ nullptr };
	
	float4 diffuse_color = float4::one;
	float3 specular_color = float3::one;
	float3 emissive_color = float3::one;
	
	float kAmbient = 0.3f;
	float kDiffuse = 0.2f;
	float kSpecular = 0.1f;
	float shininess = 32.f;

	bool changesDone = false;
	
};

#endif __Material_h__