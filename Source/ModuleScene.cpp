#include "Application.h"

#include "ModuleCamera.h"
#include "ModuleEditor.h"
#include "ModuleFileSystem.h"
#include "ModuleInput.h"
#include "ModuleProgram.h"
#include "ModuleResourceManager.h"
#include "ModuleRender.h"
#include "ModuleScene.h"
#include "ModuleTextures.h"

#include "GameObject.h"
#include "ComponentCamera.h"
#include "ComponentRenderer.h"
#include "ComponentTransform.h"

#include "Material.h"
#include "MaterialEditor.h"
#include "Mesh.h"
#include "JSON.h"
#include "myQuadTree.h"

#include "Imgui.h"
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
#include <stack>

#define MAX_DEBUG_LINES 5
#define MAX_LIGHTS 4
#define DEFAULT_SPHERE_SHAPE 20
#define QUADTREE_SIZE 20

ModuleScene::ModuleScene()
{
}


ModuleScene::~ModuleScene()
{
	RELEASE(quadtree);
}

bool ModuleScene::Init(JSON * config)
{
	pcg_extras::seed_seq_from<std::random_device> seed_source;
	pcg32 rng(seed_source);
	uuid_rng = rng;
	root = new GameObject("World", 0); //Root always has uid 0

	int size = QUADTREE_SIZE * App->renderer->current_scale;
	AABB limit(float3(-size, 0.f, -size), float3(size, 0.f, size));
	quadtree = new myQuadTree(limit);

	JSON_value* scene = config->GetValue("scene");
	if (scene != nullptr)
	{
		primitivesUID[(unsigned)PRIMITIVES::SPHERE] = scene->GetUint("sphereUID");
		primitivesUID[(unsigned)PRIMITIVES::CUBE] = scene->GetUint("cubeUID");
		ambientColor = scene->GetColor3("ambient");
		const char* dscene = scene->GetString("defaultscene");
		if (dscene != nullptr) defaultScene = dscene;
	}
	return true;
}

bool ModuleScene::Start()
{
	camera_notfound_texture = App->textures->GetTexture(NOCAMERA); 
	if (defaultScene.size() > 0)
	{
		LoadScene(defaultScene.c_str());
	}
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
	root->CleanUp();
	for (auto &child : root->children)
	{
		RELEASE(child);
	}
	root->children.clear();
	ResetQuadTree();

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
	scene->AddFloat3("ambient", ambientColor);
	scene->AddString("defaultscene", defaultScene.c_str());
	config->AddValue("scene", *scene);
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

		if (App->renderer->light_debug)
		{
			for (const auto light : lights)
			{
				light->DrawDebugLight();
			}
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

	glUniform3fv(glGetUniformLocation(shader->id,
		"lights.ambient_color"), 1, (GLfloat*)&ambientColor);
	go.SetLightUniforms(shader->id);

	go.UpdateModel(shader->id);
	crenderer->mesh->Draw(shader->id);

	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);
	glUseProgram(0);
}

void ModuleScene::DrawHierarchy()
{
	ImGui::PushStyleColor(ImGuiCol_Header,ImVec4(0.2f, 0.2f, 0.5f, 1.00f));
	root->DrawHierarchy(selected);
	ImGui::PopStyleColor();
}

void ModuleScene::DragNDrop(GameObject* go)
{
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
	{
		GameObject * dragged_go = go;
		ImGui::SetDragDropPayload("DragDropHierarchy", &dragged_go, sizeof(GameObject *), ImGuiCond_Once);
		ImGui::Text("%s", go->name.c_str());
		ImGui::EndDragDropSource();
	}
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragDropHierarchy"))
		{
			IM_ASSERT(payload->DataSize == sizeof(GameObject*));
			GameObject* dropped_go = (GameObject *)*(const int*)payload->Data;
			if (dropped_go != App->scene->root && dropped_go->parent != go && !dropped_go->IsParented(*go))
			{
				go->children.push_back(dropped_go);

				if (dropped_go->transform != nullptr)
				{
					dropped_go->transform->SetLocalToWorld();
				}
				dropped_go->parent->children.remove(dropped_go);
				dropped_go->parent = go;
				if (dropped_go->transform != nullptr)
				{
					dropped_go->transform->SetWorldToLocal(dropped_go->parent->GetGlobalTransform());
				}
			}
		}
		ImGui::EndDragDropTarget();
	}
}

void ModuleScene::DrawGUI()
{
	ImGui::ColorEdit3("Ambient", (float*)&ambientColor);
	if(ImGui::Button("Reset QuadTree"))
	{
		ResetQuadTree();
	}
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

void ModuleScene::AddToSpacePartition(GameObject *gameobject)
{
	assert(gameobject != nullptr);
	if (gameobject == nullptr)	return;

	if (gameobject->isStatic)
	{
		quadtree->Insert(gameobject);
	}
	else
	{
		dynamicGOs.insert(gameobject);
	}
}

void ModuleScene::DeleteFromSpacePartition(const GameObject &gameobject)
{
	if (gameobject.isStatic)
	{
		quadtree->Remove(gameobject);
	}
	else
	{
		dynamicGOs.erase((GameObject*) &gameobject);
	}
}

void ModuleScene::ResetQuadTree()
{
	int size = QUADTREE_SIZE * App->renderer->current_scale;
	AABB limit(float3(-size, 0.f, -size), float3(size, 0.f, size));
	quadtree->Clear(limit);
	std::stack<GameObject*> gos;
	gos.push(root);

	while (!gos.empty())
	{
		GameObject* go = gos.top();
		gos.pop();
		if (go->isStatic)
		{
			quadtree->Insert(go);
		}
	}
}

void ModuleScene::CreateCube(const char * name, GameObject* parent)
{
	if (!primitivesUID[(unsigned)PRIMITIVES::CUBE])
	{
		par_shapes_mesh* mesh = par_shapes_create_cube();
		if (mesh->normals == nullptr)
		{
			par_shapes_compute_normals(mesh);
		}
		SetPrimitiveMesh(mesh, PRIMITIVES::CUBE);
	}
	CreatePrimitive(name, parent, PRIMITIVES::CUBE);
}

void ModuleScene::CreateSphere(const char * name, GameObject* parent)
{
	if (!primitivesUID[(unsigned)PRIMITIVES::SPHERE])
	{
		par_shapes_mesh* mesh = par_shapes_create_parametric_sphere(DEFAULT_SPHERE_SHAPE, DEFAULT_SPHERE_SHAPE);
		SetPrimitiveMesh(mesh, PRIMITIVES::SPHERE);
	}
	CreatePrimitive(name, parent, PRIMITIVES::SPHERE);
}

void ModuleScene::CreatePrimitive(const char * name, GameObject* parent, PRIMITIVES type)
{
	GameObject * gameobject = CreateGameObject(name, parent);
	Select(gameobject);
	ComponentTransform* transform = (ComponentTransform*)gameobject->CreateComponent(ComponentType::Transform);
	transform->scale.SetFromScalar(App->renderer->current_scale);
	transform->UpdateTransform();
	ComponentRenderer* crenderer = (ComponentRenderer*)gameobject->CreateComponent(ComponentType::Renderer);

	unsigned uid = primitivesUID[(unsigned)type];
	char *data = nullptr;
	App->fsystem->Load((MESHES + std::to_string(uid) + MESHEXTENSION).c_str(), &data);
	crenderer->UpdateMesh(data, uid);//Deallocates data
	crenderer->SetMaterial(DEFAULTMAT);
	App->resManager->AddMesh(crenderer->mesh);
	App->scene->Select(gameobject);
}

void ModuleScene::SetPrimitiveMesh(par_shapes_mesh_s *mesh, PRIMITIVES type)
{
	par_shapes_scale(mesh, 1.f, 1.f, 1.f);
	char* data = nullptr;
	unsigned meshSize = SaveParShapesMesh(*mesh, &data);
	unsigned uid = GetNewUID();
	bool saved = App->fsystem->Save((MESHES + std::to_string(uid) + MESHEXTENSION).c_str(), data, meshSize);
	if (saved)
	{
		LOG("Primitive mesh %u saved", uid);
	}
	else
	{
		LOG("Error saving primitive mesh %u", uid);
	}
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
	json->AddValue("GameObjects", *array);
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
	name.clear();
}

void ModuleScene::Select(GameObject * gameobject)
{
	if (App->editor->materialEditor->material != nullptr)
	{
		App->editor->materialEditor->Save();
		App->editor->materialEditor->open = false;
	}

	if (selected != nullptr)
	{
		selected->drawBBox = false;
	}
	selected = gameobject;
	gameobject->drawBBox = true;
	App->editor->ShowInspector();
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
			if (go.second->Intersects(line, distance)) //returns distance to line if triangle hit
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
