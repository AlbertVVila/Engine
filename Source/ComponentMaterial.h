#ifndef __ComponentMaterial_h__
#define __ComponentMaterial_h__

#include "Component.h"

struct aiMaterial;
struct Texture;

class ComponentMaterial :
	public Component
{
public:
	ComponentMaterial(GameObject* gameobject, const aiMaterial * material = nullptr);
	~ComponentMaterial();

	void DeleteTexture();
	
	void SetMaterial(const aiMaterial * material);
	Texture * GetTexture() const;
	unsigned int GetShader();
	void DrawProperties() override;
	Component* Copy() override;

public:
	unsigned int shader = 0;
	Texture *texture = nullptr;
};

#endif //__ComponentMaterial_h__
