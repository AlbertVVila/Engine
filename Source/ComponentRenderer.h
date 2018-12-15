#ifndef __ComponentRenderer_h__
#define __ComponentRenderer_h__

#include "Component.h"
#include "Math/float4.h"

struct Texture;
struct Shader;
struct Material
{
	Shader* shader = nullptr;
	float4 color = float4::one;
	Texture* texture = nullptr;
	float kAmbient = 0.3f;
	float kDiffuse = 0.5f;
	float kSpecular = 0.3f;
	float shininess = 32.f;

	void SetShader(Shader* s)
	{
		shader = s;
	}

	void SetTexture(Texture* t)
	{
		texture = t;
	}
};
class ComponentRenderer :
	public Component
{
public:
	ComponentRenderer(GameObject* gameobject, const char * material = nullptr);
	ComponentRenderer(const ComponentRenderer& component);
	~ComponentRenderer();

	Component* Clone() override;
	void DeleteTexture();
	
	void SetTexture(const char * data);
	void SetShader(const char * shaderName);
	Texture * GetTexture() const;
	Shader* GetShader() const;
	float4 GetColor() const;
	void DrawProperties() override;
	void Save(JSON_value *value) const override;
	void Load(JSON_value *value) override;

public:
	Material* material = nullptr;
	//unsigned int shader = 0;
	//Texture *texture = nullptr;
	//float4 color = float4::one;

	//float kAmbient = 0.2f;
	//float kDiffuse = 0.5f;
	//float kSpecular = 0.5f;
	//float shininess = 64.f;

private:
	std::string selected_texture = "None selected";
	std::string selected_shader = "Default";
};

#endif //__ComponentRenderer_h__
