#ifndef __ModuleScene_h__
#define __ModuleScene_h__

#include "Module.h"
#include "Geometry/Frustum.h"
#include "pcg_random.hpp"
#include "Math/Quat.h"
#include "Math/float4.h"

class GameObject;
class ComponentCamera;
class ComponentLight;
class myQuadTree;
struct Texture;
struct par_shapes_mesh_s;

class ModuleScene :
	public Module
{
public:
	ModuleScene();
	~ModuleScene();

	bool Init() override;
	bool Start() override;
	update_status Update() override;
	bool CleanUp() override;

	GameObject * CreateGameObject(const float4x4& transform, const char* filepath, const char* name, GameObject* parent);
	GameObject * CreateGameObject(const char * name, GameObject* parent);

	void Draw(const math::Frustum &frustum);
	void DrawGO(const math::Frustum &frustum);
	void DrawHierarchy();
	void CreateCube(const char * name, GameObject* parent, const float3 & pos = float3::zero, const Quat & rot = Quat::identity, float size = 1.f, const float4 & color = float4::one);
	void CreateSphere(const char * name, GameObject* parent, const float3 & pos = float3::zero, const Quat & rot = Quat::identity, float size = 1.f, unsigned slices = 20u, unsigned stacks = 20u, const float4 & color = float4::one);
	void CreatePrimitive(par_shapes_mesh_s *mesh, const char * name, const float3 & pos, const Quat & rot, float size, const float4 & color, GameObject* parent);
	unsigned SaveParShapesMesh(const par_shapes_mesh_s & mesh, char** data) const;
	void SaveScene(const GameObject &rootGO, const char* name) const;
	void LoadScene(const char * scene);
	void ClearScene();
	void Select(GameObject* gameobject);
	void Pick(float normalized_x, float normalized_y);
	unsigned GetNewUID();
	std::list<ComponentLight*>GetClosestSpotLights(float3 position) const;
	std::list<ComponentLight*>GetClosestPointLights(float3 position) const;
	ComponentLight* GetDirectionalLight() const;

public:
	GameObject* root = nullptr;
	GameObject* selected = nullptr; //Selected in hierarchy
	ComponentCamera* maincamera = nullptr; //Released by GameObject holding it
	Texture* camera_notfound_texture = nullptr; //Released in resource manager
	std::list<LineSegment> debuglines;

	std::list<ComponentLight*> lights;
	myQuadTree * quadtree = nullptr;
	pcg32 uuid_rng;
	std::string name;
};

#endif __ModuleScene_h__