#ifndef __ComponentRenderer_h__
#define __ComponentRenderer_h__

#include "Component.h"
#include "Math/float3.h"

class ResourceMesh;
class ResourceMaterial;
struct BindBone;

class ComponentRenderer :
	public Component
{
	friend class ResourceMaterial;

public:
	ComponentRenderer(GameObject* gameobject);
	ComponentRenderer(const ComponentRenderer& component);
	~ComponentRenderer();


	Component* Clone() const override;
	void DrawProperties() override;
	bool CleanUp() override;

	void Save(JSON_value* value) const override;
	void Load(JSON_value* value) override;

	ENGINE_API void SetMaterial(const char* materialName);
	void SetMesh(const char* meshfile);
	void UpdateGameObject();
	void LinkBones();

	void DrawMesh(unsigned shaderProgram);

public:
	ResourceMesh* mesh = nullptr;
	ResourceMaterial* material = nullptr;
	bool castShadows = false;
	bool useAlpha = false;
	bool highlighted = false;
	math::float3 highlightColor = math::float3::one;

private:

	void Update();

	std::vector<std::string> guiMaterials;
	std::vector<std::string> guiMeshes;
	int xTiles = 1u;
	int yTiles = 1u;
	int f1Xpos;
	int f1Ypos;
	int f2Xpos;
	int f2Ypos;
	float fps = 24.f;
	float timer = 0.f;
	float frameMix = 0.f; 

	std::vector<BindBone> bindBones;
};

#endif __ComponentRenderer_h__