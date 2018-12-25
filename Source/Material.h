#ifndef __Material_h__
#define __Material_h__

#include "Math/float4.h"
#include <list>

#define MAXTEXTURES 4
#define DEFAULTMAT "Default"

struct Shader;
struct Texture;

enum class TextureType
{
	DIFFUSE = 0,
	SPECULAR,
	OCCLUSION,
	EMISSIVE
};

class Material
{
public:
	Material();
	~Material();

	void Load(const char * material);
	void Save() const;
	bool CleanUp();

	//void DeleteTexture();

	/*void SetTexture(const char * data, TextureType type);
	void SetShader(const char * shaderName);*/

	Texture * GetTexture(TextureType type) const;
	//Shader* GetShader() const;

	std::list<Texture*> GetTextures() const;
	void SetMaterial(const char * file = nullptr);
	void SetMaterial(const Material & material);

public:
	std::string name = "Default";
	Shader* shader = nullptr;
	
	Texture* textures[MAXTEXTURES]{ nullptr }; //TODO: default specular texture?
	
	float4 diffuse_color = float4::one;
	float3 specular_color = float3::one;
	float3 emissive_color = float3::one;
	
	float kAmbient = 0.3f;
	float kDiffuse = 0.2f;
	float kSpecular = 0.1f;
	float shininess = 32.f;
	
};

#endif __Material_h__