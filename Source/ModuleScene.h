#ifndef __ModuleScene_h__
#define __ModuleScene_h__

#include "Module.h"
#include "ComponentLight.h"

#include "Geometry/Frustum.h"
#include "pcg_random.hpp"
#include "Math/Quat.h"
#include "Math/float4.h"
#include <set>

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

	bool Init(JSON * config) override;
	bool Start() override;
	update_status Update(float dt) override;
	bool CleanUp() override;

	GameObject * CreateGameObject(const float4x4& transform, const char* name, GameObject* parent);
	GameObject * CreateGameObject(const char * name, GameObject* parent);

	void Draw(const math::Frustum &frustum, bool isEditor = false);
	void DrawGO(const GameObject& go, const math::Frustum & frustum, bool isEditor = false);
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
	std::list<ComponentLight*> GetClosestLights(LightType type, float3 position = float3::zero) const;

	ComponentLight * GetDirectionalLight() const;

private:
	std::list<std::pair<float, GameObject*>>GetDynamicIntersections(const LineSegment& line);
public:
	GameObject* root = nullptr;
	GameObject* selected = nullptr; //Selected in hierarchy
	ComponentCamera* maincamera = nullptr; //Released by GameObject holding it
	Texture* camera_notfound_texture = nullptr; //Released in resource manager
	std::list<LineSegment> debuglines;

	std::list<ComponentLight*> lights;
	myQuadTree * quadtree = nullptr;
	std::set<GameObject*> dynamicGOs;
	pcg32 uuid_rng;
	std::string name;
};

#endif __ModuleScene_h__