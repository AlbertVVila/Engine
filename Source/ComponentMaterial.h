#ifndef __ComponentMaterial_h__
#define __ComponentMaterial_h__

#include "Component.h"
#include "Math/float4.h"

struct Texture;

class ComponentMaterial :
	public Component
{
public:
	ComponentMaterial(GameObject* gameobject, const char * material = nullptr);
	ComponentMaterial(const ComponentMaterial& component);
	~ComponentMaterial();

	Component* Clone() override;
	void DeleteTexture();
	
	void SetMaterial(const char * data);
	Texture * GetTexture() const;
	unsigned int GetShader() const;
	float4 GetColor() const;
	void DrawProperties() override;
	void Save(JSON_value *value) const override;
	void Load(JSON_value *value) override;

public:
	unsigned int shader = 0;
	std::string file;
	Texture *texture = nullptr;
	float4 color = float4::one;

	float kAmbient = 0.2f;
	float kDiffuse = 0.5f;
	float kSpecular = 0.5f;
	float shininess = 64.f;

private:
	float4 iDiffuse = float4::zero;
};

#endif //__ComponentMaterial_h__
