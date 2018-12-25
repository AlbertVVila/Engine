#ifndef __ComponentRenderer_h__
#define __ComponentRenderer_h__

#include "Component.h"

class Mesh;
class Material;

class ComponentRenderer :
	public Component
{
public:
	ComponentRenderer(GameObject* gameobject);
	ComponentRenderer(const ComponentRenderer& component);
	~ComponentRenderer();


	Component* Clone() const override;
	void DrawProperties() override;

	void Save(JSON_value *value) const override;
	void Load(const JSON_value &value) override;

	void SetMesh(unsigned uid);
	void SetMaterial(const char* material);

public:
	Mesh * mesh = nullptr;
	Material * material = nullptr;
};

#endif __ComponentRenderer_h__