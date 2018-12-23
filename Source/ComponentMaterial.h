#ifndef __ComponentMaterial_h__
#define __ComponentMaterial_h__

#include "Component.h"
#include "Math/float3.h"
#include "Math/float4.h"
#include <list>

#define MAXTEXTURES 4
struct Texture;
struct Shader;

enum class TextureType
{
	DIFFUSE=0,
	SPECULAR,
	OCCLUSION,
	EMISSIVE
};

class ComponentMaterial :
	public Component
{
public:
	ComponentMaterial(GameObject* gameobject);
	ComponentMaterial(const ComponentMaterial& component);
	~ComponentMaterial();

	Component* Clone() const override;
	void DeleteTexture();
	
	void SetTexture(const char * data, TextureType type);
	void SetShader(const char * shaderName);
	Texture * GetTexture(TextureType type) const;
	Shader* GetShader() const;
	void DrawProperties() override;
	void Save(JSON_value *value) const override;
	void Load(const JSON_value &value) override;

	void Load(const char * material);
	void Save() const;
	std::list<Texture*> GetTextures() const;


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


private:
	//std::string selected_texture[MAXTEXTURES]{ "None selected" };
	//std::string selected_shader = "Default";
};

#endif //__ComponentMaterial_h__
