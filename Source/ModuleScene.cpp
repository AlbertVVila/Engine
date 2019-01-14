#include "Application.h"

#include "ModuleScene.h"
#include "ModuleCamera.h"
#include "ModuleFileSystem.h"
#include "ModuleProgram.h"
#include "ModuleTextures.h"
#include "ModuleResourceManager.h"
#include "ModuleRender.h"

#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentRenderer.h"
#include "ComponentCamera.h"

#include "Material.h"
#include "Mesh.h"
#include "JSON.h"
#include "myQuadTree.h"
#include "Geometry/LineSegment.h"
#include "Math/MathConstants.h"
#include "GL/glew.h"
#include "Brofiler.h"


#pragma warning(push)
#pragma warning(disable : 4996)  
#pragma warning(disable : 4244)  
#pragma warning(disable : 4305)  
#pragma warning(disable : 4838)  
#define PAR_SHAPES_IMPLEMENTATION
#include "par_shapes.h"
#pragma warning(pop)

#include <random>
#include <map>

#define MAX_DEBUG_LINES 5
#define MAX_LIGHTS 4
#define DEFAULT_SPHERE_SHAPE 20

ModuleScene::ModuleScene()
{
}


ModuleScene::~ModuleScene()
{
}

bool ModuleScene::Init(JSON * config)
{
	pcg_extras::seed_seq_from<std::random_device> seed_source;
	pcg32 rng(seed_source);
	uuid_rng = rng;
	root = new GameObject("World", 0); //Root always has uid 0

	AABB limits(float3(-10.f, 0.f, -10.f), float3(10.f, 0.f, 10.f));
	quadtree = new myQuadTree(limits);

	JSON_value* scene = config->GetValue("scene");
	if (scene != nullptr)
	{
		primitivesUID[(unsigned)PRIMITIVES::SPHERE] = scene->GetUint("sphereUID");
		primitivesUID[(unsigned)PRIMITIVES::CUBE] = scene->GetUint("cubeUID");
	}
	return true;
}

bool ModuleScene::Start()
{
	camera_notfound_texture = App->textures->GetTexture(NOCAMERA); 
	return true;
}

update_status ModuleScene::Update(float dt)
{
	BROFILER_CATEGORY("Scene Update", Profiler::Color::Green);
	root->Update();
	return UPDATE_CONTINUE;
}

bool ModuleScene::CleanUp()
{
	quadtree->Clear();
	root->CleanUp();
	for (auto &child : root->children)
	{
		RELEASE(child);
	}
	root->children.clear();

	selected = nullptr;
	maincamera = nullptr;

	App->resManager->DeleteTexture(camera_notfound_texture->file);
	camera_notfound_texture = nullptr;

	lights.clear();

	return true;
}

void ModuleScene::SaveConfig(JSON * config)
{
	JSON_value* scene = config->CreateValue();

	scene->AddUint("sphereUID", primitivesUID[(unsigned)PRIMITIVES::SPHERE]);
	scene->AddUint("cubeUID", primitivesUID[(unsigned)PRIMITIVES::CUBE]);

	config->AddValue("scene", scene);
}

void ModuleScene::Draw(const Frustum &frustum, bool isEditor)
{
	PROFILE;
	if (isEditor)
	{
		if (App->renderer->quadtree_debug)
		{
			quadtree->Draw();
		}

		for (const auto light : lights)
		{
			light->DrawDebugLight();
		}
	}
	Frustum camFrustum = frustum;
	if (maincamera != nullptr && App->renderer->useMainCameraFrustum)
	{
		camFrustum = *maincamera->frustum;
	}
	std::list<GameObject*> staticGOs = quadtree->GetIntersections(camFrustum);
	for (const auto &go : staticGOs)
	{
		DrawGO(*go, camFrustum, isEditor);
	}

	for (const auto &go : dynamicGOs)
	{
		if (camFrustum.Intersects(go->GetBoundingBox()))
		{
			DrawGO(*go, camFrustum, isEditor);
		}
	}

	if (selected != nullptr && selected->GetComponent(ComponentType::Renderer) == nullptr)
	{
		DrawGO(*selected, frustum, isEditor); //bcause it could be an object without mesh not in staticGOs or dynamicGOs
	}
}

void ModuleScene::DrawGO(const GameObject& go, const Frustum & frustum, bool isEditor)
{
	PROFILE;
	if (go.drawBBox && isEditor)
	{
		go.DrawBBox();
	}

	ComponentRenderer* crenderer = (ComponentRenderer*)go.GetComponent(ComponentType::Renderer);
	if (crenderer == nullptr || !crenderer->enabled || crenderer->material == nullptr) return;

	Material* material = crenderer->material;
	Shader* shader = material->shader;
	if (shader == nullptr) return;

	glUseProgram(shader->id);

	material->SetUniforms(shader->id);
	go.SetLightUniforms(shader->id);

	go.UpdateModel(shader->id);
	crenderer->mesh->Draw(shader->id);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}

void ModuleScene::DrawHierarchy()
{
	root->DrawHierarchy(selected);
}

GameObject * ModuleScene::CreateGameObject(const float4x4 & transform, const char * name, GameObject* parent)
{
	GameObject * gameobject = new GameObject(transform, name, GetNewUID());
	if (parent != nullptr)
	{
		gameobject->parent = parent;
		parent->children.push_back(gameobject);
	}
	return gameobject;
}

GameObject * ModuleScene::CreateGameObject(const char * name, GameObject* parent)
{
	GameObject * gameobject = new GameObject(name, GetNewUID());
	if (parent != nullptr)
	{
		gameobject->parent = parent;
		parent->children.push_back(gameobject);
	}
	return gameobject;
}

void ModuleScene::CreateCube(const char * name, GameObject* parent, float size)
{
	if (!primitivesUID[(unsigned)PRIMITIVES::CUBE])
	{
		par_shapes_mesh* mesh = par_shapes_create_cube();
		if (mesh->normals == nullptr)
		{
			par_shapes_compute_normals(mesh);
		}
		SetPrimitiveMesh(mesh, size, PRIMITIVES::CUBE);
	}
	CreatePrimitive(name, parent, PRIMITIVES::CUBE);
}

void ModuleScene::CreateSphere(const char * name, GameObject* parent, float size)
{
	if (!primitivesUID[(unsigned)PRIMITIVES::SPHERE])
	{
		par_shapes_mesh* mesh = par_shapes_create_parametric_sphere(DEFAULT_SPHERE_SHAPE, DEFAULT_SPHERE_SHAPE);
		SetPrimitiveMesh(mesh, size, PRIMITIVES::SPHERE);
	}
	CreatePrimitive(name, parent, PRIMITIVES::SPHERE);
}

void ModuleScene::CreatePrimitive(const char * name, GameObject* parent, PRIMITIVES type)
{
	GameObject * gameobject = CreateGameObject(name, parent);
	Select(gameobject);
	ComponentTransform* transform = (ComponentTransform*)gameobject->CreateComponent(ComponentType::Transform);
	ComponentRenderer* crenderer = (ComponentRenderer*)gameobject->CreateComponent(ComponentType::Renderer);

	unsigned uid = primitivesUID[(unsigned)type];
	char *data = nullptr;
	App->fsystem->Load((MESHES + std::to_string(uid) + MESHEXTENSION).c_str(), &data);
	crenderer->SetMesh(data, uid);//Deallocates data
	crenderer->SetMaterial(DEFAULTMAT);
	gameobject->UpdateBBox();
	App->resManager->AddMesh(crenderer->mesh);
}

void ModuleScene::SetPrimitiveMesh(par_shapes_mesh_s *mesh, float size, PRIMITIVES type)
{
	par_shapes_scale(mesh, size*App->renderer->current_scale, size*App->renderer->current_scale, size*App->renderer->current_scale);
	char* data = nullptr;
	unsigned meshSize = SaveParShapesMesh(*mesh, &data);
	unsigned uid = GetNewUID();
	App->fsystem->Save((MESHES + std::to_string(uid) + MESHEXTENSION).c_str(), data, meshSize);
	primitivesUID[(unsigned)type] = uid;
	par_shapes_free_mesh(mesh);
}

unsigned ModuleScene::SaveParShapesMesh(const par_shapes_mesh_s &mesh, char** data) const //TODO: unify somehow with importer
{
	unsigned size = 0;
	unsigned ranges[2] = { mesh.ntriangles*3, mesh.npoints};
	size += sizeof(ranges); //numfaces + numvertices
	size += ranges[0] * 3 * sizeof(int); //indices
	
	size += sizeof(float)*ranges[1] * 3;
	size += sizeof(bool) * 2; //has normals + has tcoords
	if (mesh.normals != nullptr)
	{
		size += sizeof(float)*ranges[1] * 3;
	}
	if (mesh.tcoords != nullptr)
	{
		size += sizeof(float)*ranges[1] * 2;
	}

	*data = new char[size];
	char *cursor = *data;

	unsigned rangeBytes = sizeof(ranges); 
	memcpy(cursor, ranges, rangeBytes);
	cursor += rangeBytes;

	unsigned verticesBytes = sizeof(float)*mesh.npoints * 3;
	memcpy(cursor, mesh.points, verticesBytes);
	cursor += verticesBytes;

	bool hasNormals = mesh.normals != nullptr ? true : false;
	memcpy(cursor, &hasNormals, sizeof(bool));
	cursor += sizeof(bool);

	if (hasNormals)
	{
		unsigned normalsBytes = sizeof(float)*mesh.npoints * 3;
		memcpy(cursor, mesh.normals, normalsBytes);
		cursor += normalsBytes;
	}

	bool hasTcoords = mesh.tcoords != nullptr ? true : false;
	memcpy(cursor, &hasTcoords, sizeof(bool));
	cursor += sizeof(bool);

	if (hasTcoords)
	{
		unsigned tcoordsBytes = sizeof(float)*mesh.npoints * 2;
		memcpy(cursor, mesh.tcoords, tcoordsBytes);
		cursor += tcoordsBytes;
	}

	short mask = 0;
	for (int i = 0; i < mesh.ntriangles*3; i++)
	{
		memcpy(cursor, &mesh.triangles[i], sizeof(short)); 
		cursor += sizeof(short);
		memcpy(cursor, &mask, sizeof(short));
		cursor += sizeof(short); //implicitly converting to unsigned
	}

	return size;
}


void ModuleScene::SaveScene(const GameObject &rootGO, const char* filename) const
{
	JSON *json = new JSON();
	JSON_value *array =json->CreateValue(rapidjson::kArrayType);
	rootGO.Save(array);
	json->AddValue("GameObjects", array);
	std::string file(filename);
	App->fsystem->Save((SCENES + file + JSONEXT).c_str(), json->ToString().c_str(), json->Size());
	RELEASE(json);
}

void ModuleScene::LoadScene(const char* scene)
{
	ClearScene();
	AddScene(scene);
	name = scene;
}

void ModuleScene::AddScene(const char * scene)
{
	char* data = nullptr;
	std::string sceneName(scene);
	App->fsystem->Load((SCENES + sceneName + JSONEXT).c_str(), &data);

	JSON *json = new JSON(data);
	JSON_value* gameobjectsJSON = json->GetValue("GameObjects");
	std::map<unsigned, GameObject*> gameobjectsMap; //Necessary to assign parent-child efficiently

	for (unsigned i = 0; i<gameobjectsJSON->Size(); i++)
	{
		JSON_value* gameobjectJSON = gameobjectsJSON->GetValue(i);
		GameObject *gameobject = new GameObject();
		gameobject->Load(gameobjectJSON);
		gameobjectsMap.insert(std::pair<unsigned, GameObject*>(gameobject->UUID, gameobject));

		std::map<unsigned, GameObject*>::iterator it = gameobjectsMap.find(gameobject->parentUUID);
		if (it != gameobjectsMap.end())
		{
			gameobject->parent = it->second;
			gameobject->parent->children.push_back(gameobject);
		}
		else if (gameobject->parentUUID == 0)
		{
			gameobject->parent = root;
			gameobject->parent->children.push_back(gameobject);
		}
	}

	RELEASE_ARRAY(data);
	RELEASE(json);
}

void ModuleScene::ClearScene()
{
	CleanUp();
	camera_notfound_texture = App->textures->GetTexture(NOCAMERA);

}

void ModuleScene::Select(GameObject * gameobject)
{
	if (selected != nullptr)
	{
		selected->drawBBox = false;
	}
	selected = gameobject;
	gameobject->drawBBox = true;
}

void ModuleScene::UnSelect()
{
	if (selected != nullptr)
	{
		selected->drawBBox = false;
	}
	selected = nullptr;
}

void ModuleScene::Pick(float normalized_x, float normalized_y)
{
	LineSegment line = App->camera->editorcamera->DrawRay(normalized_x, normalized_y);
	std::list<std::pair<float, GameObject*>> GOs = quadtree->GetIntersections(line);
	std::list<std::pair<float, GameObject*>> dynamicGOs = GetDynamicIntersections(line);
	GOs.merge(dynamicGOs);

	float closestTriangle = FLOAT_INF;
	GameObject * closestGO = nullptr;

	for (const auto& go : GOs)
	{
		if (closestGO != nullptr && !go.second->GetBoundingBox().Intersects(closestGO->GetBoundingBox())) //if no intersection between AABB no need to check triangles
		{
			break;
		}
		else
		{
			float distance = FLOAT_INF;
			if (go.second->Intersects(line, &distance)) //returns distance to line if triangle hit
			{
				if (distance < closestTriangle)
				{
					closestGO = go.second;
					closestTriangle = distance;
				}
			}
		}
	}

	if (closestGO != nullptr)
	{
		Select(closestGO);
	}
	else
	{
		UnSelect();
	}

	debuglines.push_back(line);
	if (debuglines.size() > MAX_DEBUG_LINES)
	{
		debuglines.erase(debuglines.begin());
	}
}

unsigned ModuleScene::GetNewUID()
{
	return uuid_rng();
}

std::list<ComponentLight*> ModuleScene::GetClosestLights(LightType type, float3 position) const
{
	std::map<float, ComponentLight*> lightmap;
	for (const auto& light : lights)
	{
		if (light->lightType == type && light->enabled && light->gameobject->transform != nullptr)
		{
			float distance = light->gameobject->transform->position.Distance(position);
			lightmap.insert(std::pair<float, ComponentLight*>(distance,light));
		}
	}

	std::list<ComponentLight*> closest;
	int i = 0;
	for (const auto& light : lightmap)
	{
		closest.push_back(light.second);
		++i;
		if (i == MAX_LIGHTS) break;
	}
	return closest;
}

ComponentLight* ModuleScene::GetDirectionalLight() const
{
	for (const auto& light : lights)
	{
		if (light->lightType == LightType::DIRECTIONAL && light->enabled)
		{
			return light;
		}
	}
	return nullptr;
}

std::list<std::pair<float, GameObject*>> ModuleScene::GetDynamicIntersections(const LineSegment & line)
{
	std::list<std::pair<float, GameObject*>> gos; //TODO: reusable code in quadtree intersection
	for (const auto &go : dynamicGOs)
	{
		float dNear = -FLOAT_INF;
		float dFar = FLOAT_INF;
		if (line.Intersects(go->GetBoundingBox(), dNear, dFar))
		{
			gos.push_back(std::pair<float, GameObject*>(dNear, go));
		}
	}
	gos.sort();
	return gos;
}
