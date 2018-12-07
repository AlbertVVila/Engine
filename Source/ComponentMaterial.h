#ifndef __ComponentMaterial_h__
#define __ComponentMaterial_h__

#include "Component.h"
#include "Math/float4.h"
struct aiMaterial;
struct Texture;

class ComponentMaterial :
	public Component
{
public:
	ComponentMaterial(GameObject* gameobject, const aiMaterial * material = nullptr);
	ComponentMaterial(const ComponentMaterial& component);
	~ComponentMaterial();

	Component* Clone() override;
	void DeleteTexture();
	
	void SetMaterial(const aiMaterial * material);
	Texture * GetTexture() const;
	unsigned int GetShader() const;
	float4 GetColor() const;
	void DrawProperties() override;
	void Save(JSON_value *value) override;
	void Load(JSON_value *value) override;

public:
	unsigned int shader = 0;
	Texture *texture = nullptr;
	float4 color = float4::one;
};

#endif //__ComponentMaterial_h__
