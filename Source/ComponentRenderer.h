#ifndef __ComponentRenderer_h__
#define __ComponentRenderer_h__

#include "Component.h"
#include "Math/float4.h"
#include <list>

struct Texture;
struct Shader;

enum class TextureType
{
	DIFFUSE=0,
	SPECULAR,
	OCCLUSION,
	EMISSIVE
};
struct Material
{
	std::string name;
	Shader* shader = nullptr;
	float4 color = float4::one;

	Texture* diffuseTexture = nullptr;
	Texture* specularTexture = nullptr;
	Texture* occlusionTexture = nullptr;
	Texture* emissiveTexture = nullptr;

	float kAmbient = 0.3f;
	float kDiffuse = 0.5f;
	float kSpecular = 0.3f;
	float shininess = 32.f;

	void SetShader(Shader* s)
	{
		shader = s;
	}

	void SetTexture(Texture* t, TextureType type)
	{
		switch (type)
		{
			case TextureType::DIFFUSE:
				diffuseTexture = t;
				break;
			case TextureType::SPECULAR:
				specularTexture = t;
				break;
			case TextureType::OCCLUSION:
				occlusionTexture = t;
				break;
			case TextureType::EMISSIVE:
				emissiveTexture = t;
				break;
		}
	}

	std::list<Texture*> GetTextures() const
	{
		std::list<Texture*> textures;
		textures.push_back(diffuseTexture);
		textures.push_back(specularTexture);
		textures.push_back(occlusionTexture);
		textures.push_back(emissiveTexture);
		return textures;
	}
};

class ComponentRenderer :
	public Component
{
public:
	ComponentRenderer(GameObject* gameobject);
	ComponentRenderer(const ComponentRenderer& component);
	~ComponentRenderer();

	Component* Clone() override;
	void DeleteTexture();
	
	void SetTexture(const char * data, TextureType type);
	void SetShader(const char * shaderName);
	Texture * GetTexture(TextureType type) const;
	Shader* GetShader() const;
	float4 GetColor() const;
	void DrawProperties() override;
	void Save(JSON_value *value) const override;
	void Load(JSON_value *value) override;

	void LoadMaterial(const char * material);

	void SaveMaterial(JSON_value * materialJSON) const;

public:
	Material* material = nullptr;

private:
	std::string selected_texture = "None selected";
	std::string selected_shader = "Default";
};

#endif //__ComponentRenderer_h__
