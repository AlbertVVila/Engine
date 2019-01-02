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

	void SetMaterial(const char* material);
	void SetMesh(const char* meshData, unsigned UID); //TODO: should be const data

public:
	Mesh * mesh = nullptr;
	Material * material = nullptr;
};

#endif __ComponentRenderer_h__