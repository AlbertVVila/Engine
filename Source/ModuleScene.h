#ifndef __ModuleScene_h__
#define __ModuleScene_h__

#include "Module.h"
#include "ComponentLight.h"

#include "Geometry/Frustum.h"
#include "pcg_random.hpp"
#include "Math/Quat.h"
#include "Math/float4.h"
#include <set>

#define NBPRIMITIVES 2
class GameObject;
class ComponentCamera;
class ComponentLight;
class myQuadTree;
struct Texture;
struct par_shapes_mesh_s;

enum class PRIMITIVES
{
	SPHERE,
	CUBE,
};

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
	void SaveConfig(JSON * config) override;

	GameObject * CreateGameObject(const char * name, GameObject* parent);

	void AddToSpacePartition(GameObject * gameobject) const;
	void ResetQuadTree();

	void Draw(const Frustum &frustum, bool isEditor = false);
	void DrawGO(const GameObject& go, const Frustum & frustum, bool isEditor = false);
	void DrawHierarchy();

	void CreateCube(const char * name, GameObject* parent);
	void CreateSphere(const char * name, GameObject* parent);
	void CreatePrimitive(const char * name, GameObject* parent, PRIMITIVES type);
	void SetPrimitiveMesh(par_shapes_mesh_s * mesh, PRIMITIVES type);
	unsigned SaveParShapesMesh(const par_shapes_mesh_s & mesh, char** data) const;

	void SaveScene(const GameObject &rootGO, const char* name) const;
	void LoadScene(const char * scene);
	void AddScene(const char* scene);
	void ClearScene();

	void Select(GameObject* gameobject);
	void UnSelect();
	void Pick(float normalized_x, float normalized_y);

	unsigned GetNewUID();
	std::list<ComponentLight*> GetClosestLights(LightType type, float3 position = float3::zero) const;

	ComponentLight * GetDirectionalLight() const;

private:
	std::list<std::pair<float, GameObject*>>GetDynamicIntersections(const LineSegment& line);
	unsigned primitivesUID[NBPRIMITIVES] = {0};

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