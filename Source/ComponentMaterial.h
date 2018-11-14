#ifndef __ComponentMaterial_h__
#define __ComponentMaterial_h__

#include "Component.h"

struct aiMaterial;
struct Texture;

class ComponentMaterial :
	public Component
{
public:
	ComponentMaterial(const aiMaterial * material = nullptr);
	~ComponentMaterial();
	
	void SetMaterial(const aiMaterial * material);
	Texture * GetTexture() const;
public:
	unsigned int shader;
	Texture *texture = nullptr;
};

#endif //__ComponentMaterial_h__
