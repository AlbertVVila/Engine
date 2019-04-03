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
#include "ModuleSpacePartitioning.h"

#include "GameObject.h"
#include "ComponentCamera.h"
#include "ComponentRenderer.h"
#include "ComponentTransform.h"

#include "ResourceTexture.h"
#include "ResourceMesh.h"
#include "ResourceMaterial.h"

#include "MaterialEditor.h"

#include "JSON.h"
#include "myQuadTree.h"
#include "AABBTree.h"
#include "KDTree.h"

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
#include <string>

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
	canvas = new GameObject("Canvas", 1);
	root->InsertChild(canvas);
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
		defaultScene = dscene;
	}
	return true;
}

bool ModuleScene::Start()
{
	camera_notfound_texture = (ResourceTexture*)App->resManager->Get(NOCAMERA);
	if (defaultScene.size() > 0)
	{
		path = SCENES;
		//LoadScene(defaultScene.c_str(), path.c_str());
	}
	return true;
}

update_status ModuleScene::PreUpdate()
{
	FrustumCulling(*App->camera->editorcamera->frustum);
	return UPDATE_CONTINUE;
}

update_status ModuleScene::Update(float dt)
{
	BROFILER_CATEGORY("Scene Update", Profiler::Color::Green);
	root->Update();
	if (photoTimer > 0)
	{
		photoTimer -= dt;
	}

	if ((App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_DOWN && App->input->GetKey(SDL_SCANCODE_Z) == KEY_REPEAT)
		|| (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_Z) == KEY_DOWN))
	{
		RestoreLastPhoto();
	}
	if ((App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_DOWN && App->input->GetKey(SDL_SCANCODE_Y) == KEY_REPEAT)
		|| (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_Y) == KEY_DOWN))
	{
		Redo();
	}
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
	
	LOG("Reset volumetric AABBTree");
	App->spacePartitioning->aabbTree.Reset();
	LOG("Reset lighting AABBTree");
	App->spacePartitioning->aabbTreeLighting.Reset();

	selected = nullptr;
	maincamera = nullptr;

	App->resManager->DeleteResource(camera_notfound_texture->GetUID());
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

void ModuleScene::FrustumCulling(const Frustum & frustum)
{
	Frustum camFrustum = frustum;

	if (maincamera != nullptr && App->renderer->useMainCameraFrustum)
	{
		camFrustum = *maincamera->frustum;
	}
	App->spacePartitioning->kDTree.GetIntersections(camFrustum, staticFilteredGOs);
	App->spacePartitioning->aabbTree.GetIntersections(camFrustum, dynamicFilteredGOs);
}

void ModuleScene::Draw(const Frustum &frustum, bool isEditor)
{
	PROFILE;
	if (isEditor)
	{
		if (App->renderer->aabbTreeDebug)
		{
			App->spacePartitioning->aabbTree.Draw();
		}
		if (App->renderer->kDTreeDebug)
		{
			App->spacePartitioning->kDTree.DebugDraw();
		}

		if (App->renderer->light_debug)
		{
			for (const auto &light : lights)
			{
				light->DrawDebugLight();
			}
			App->spacePartitioning->aabbTreeLighting.Draw();
		}
	}
	Frustum camFrustum = frustum;
	if (maincamera != nullptr && App->renderer->useMainCameraFrustum)
	{
		camFrustum = *maincamera->frustum;
	}
	
	for (const auto &go : staticFilteredGOs)
	{
		if (!camFrustum.Intersects(go->GetBoundingBox()))
		{
			DrawGO(*go, camFrustum, isEditor);
		}
	}

	for (const auto &go : dynamicFilteredGOs)
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
		if (go.light != nullptr)
		{
			go.light->DrawDebug();
		}
	}
	ComponentRenderer* crenderer = (ComponentRenderer*)go.GetComponent(ComponentType::Renderer);
	if (crenderer == nullptr || !crenderer->enabled || crenderer->material == nullptr) return;

	ResourceMaterial* material = crenderer->material;
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
	ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.2f, 0.2f, 0.5f, 1.00f));
	root->DrawHierarchy();
	ImGui::PopStyleColor();
}

void ModuleScene::DragNDropMove(GameObject* target) 
{
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragDropHierarchy"))
		{
			IM_ASSERT(payload->DataSize == sizeof(GameObject*));
			TakePhoto();
			GameObject* droppedGo = (GameObject *)*(const int*)payload->Data;
			if (droppedGo != App->scene->root && target != droppedGo)
			{
				for (GameObject* droppedGo : App->scene->selection)
				{
					droppedGo->parent->children.remove(droppedGo);

					std::list<GameObject*>::iterator it = std::find(target->parent->children.begin(), target->parent->children.end(), target);

					target->parent->children.insert(it, droppedGo);

					if (droppedGo->transform != nullptr)
					{
						droppedGo->transform->SetLocalToWorld();
					}

					droppedGo->parent = target->parent;
					if (droppedGo->transform != nullptr)
					{
						droppedGo->transform->SetWorldToLocal(droppedGo->parent->GetGlobalTransform());
					}
				}
			}
		}
		ImGui::EndDragDropTarget();
	}
}

void ModuleScene::DragNDrop(GameObject* go)
{
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
	{
		GameObject * dragged_go = go;
		ImGui::SetDragDropPayload("DragDropHierarchy", &dragged_go, sizeof(GameObject *), ImGuiCond_Once);
		for (GameObject* selectionGO : selection)
			ImGui::Text("%s", selectionGO->name.c_str());
		ImGui::EndDragDropSource();
	}
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragDropHierarchy"))
		{
			IM_ASSERT(payload->DataSize == sizeof(GameObject*));
			GameObject* droppedGo = (GameObject *)*(const int*)payload->Data;
			if (droppedGo != App->scene->root && droppedGo->parent != go && !droppedGo->IsParented(*go) 
				&& std::find(App->scene->selection.begin(), App->scene->selection.end(), go) == App->scene->selection.end())
			{
				TakePhoto();
				for (GameObject* droppedGo : App->scene->selection)
				{
					go->children.push_back(droppedGo);

					if (droppedGo->transform != nullptr)
					{
						droppedGo->transform->SetLocalToWorld();
					}
					droppedGo->parent->children.remove(droppedGo);
					droppedGo->parent = go;
					if (droppedGo->transform != nullptr)
					{
						droppedGo->transform->SetWorldToLocal(droppedGo->parent->GetGlobalTransform());
					}
				}
			}
		}
		ImGui::EndDragDropTarget();
	}
}

void ModuleScene::DrawGUI()
{
	ImGui::ColorEdit3("Ambient", (float*)&ambientColor);
	if (ImGui::InputInt("KdTree bucket size", &App->spacePartitioning->kDTree.bucketSize))
	{
		if (App->spacePartitioning->kDTree.bucketSize <= 0)
		{
			App->spacePartitioning->kDTree.bucketSize = 1;
		}
		App->spacePartitioning->kDTree.Calculate();
	}
	if (ImGui::InputInt("KdTree max depth", &App->spacePartitioning->kDTree.maxDepth))
	{
		if (App->spacePartitioning->kDTree.maxDepth <= 0)
		{
			App->spacePartitioning->kDTree.maxDepth = 1;
		}
		App->spacePartitioning->kDTree.Calculate();
	}
	if(ImGui::Button("Reset kdTree"))
	{
		App->spacePartitioning->kDTree.Calculate();
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
		if (gameobject->isVolumetric)
		{
			staticGOs.insert(gameobject);
		}
	}
	else
	{
		if (gameobject->isVolumetric)
		{
			App->spacePartitioning->aabbTree.InsertGO(gameobject);
		}
	}
}

void ModuleScene::DeleteFromSpacePartition(GameObject* gameobject)
{
	if (gameobject->isStatic && gameobject->isVolumetric)
	{
		staticGOs.erase(gameobject);
		App->spacePartitioning->kDTree.Calculate();
	}
	else
	{
		if (gameobject->isVolumetric && gameobject->treeNode != nullptr)
		{
			App->spacePartitioning->aabbTree.ReleaseNode(gameobject->treeNode);
		}
		if (gameobject->hasLight && gameobject->treeNode != nullptr)
		{
			App->spacePartitioning->aabbTreeLighting.ReleaseNode(gameobject->treeNode);
		}
	}
	dynamicFilteredGOs.erase(gameobject);
	staticFilteredGOs.erase(gameobject);
}

void ModuleScene::ResetQuadTree() //deprecated
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
	// TODO [MeshRefactor] - Refactor sphere/Cube creation
	//char *data = nullptr;
	//App->fsystem->Load((MESHES + std::to_string(uid) + MESHEXTENSION).c_str(), &data);
	//crenderer->UpdateMesh(data, uid);//Deallocates data
	App->resManager->Get(uid);
	crenderer->UpdateGameObject();
	crenderer->SetMaterial(DEFAULTMAT);
	//App->resManager->AddMesh(crenderer->mesh);
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

void ModuleScene::SaveScene(const GameObject& rootGO, const char* scene, const char* scenePath)
{
	JSON *json = new JSON();
	JSON_value *array =json->CreateValue(rapidjson::kArrayType);
	rootGO.Save(array);
	json->AddValue("GameObjects", *array);

	std::string file(scenePath);
	file += scene;
	file += JSONEXT;

	App->fsystem->Save(file.c_str(), json->ToString().c_str(), json->Size());
	RELEASE(json);

	// Update scene info
	name = scene;
	path = scenePath;
}
void ModuleScene::TakePhoto()
{
	TakePhoto(scenePhotos);
	scenePhotosUndoed.clear();
}

void ModuleScene::TakePhoto(std::list<GameObject*>& target)
{
	photoTimer = TIME_BETWEEN_PHOTOS;
	photoEnabled = true;	
	target.push_back(new GameObject(*root));
	if (target.size() > MAX_PHOTOS)
	{
		RELEASE(target.front());
		target.pop_front();
	}
	photoEnabled = false;	
}
void ModuleScene::RestorePhoto(GameObject* photo)
{
	photoTimer = 0.f;
	root = photo;
	std::stack<GameObject*> goStack;
	goStack.push(root);
	while (!goStack.empty())
	{
		GameObject* go = goStack.top(); goStack.pop();

		for (Component* comp : go->components)
		{
			switch (comp->type)
			{
			case ComponentType::Renderer:
				if (!go->isStatic)
				{
					App->spacePartitioning->aabbTree.InsertGO(go);
				}
				else
				{
					staticGOs.insert(go);
					App->spacePartitioning->kDTree.Calculate();
				}
				go->isVolumetric = true;
				break;
			case ComponentType::Light:
				go->light = (ComponentLight*)comp;
				go->light->CalculateGuizmos();
				App->spacePartitioning->aabbTreeLighting.InsertGO(go);
				go->hasLight = true;
				lights.push_back((ComponentLight*)comp);
				break;
			case ComponentType::Camera:
				if (((ComponentCamera*)comp)->isMainClone)
				{
					maincamera = (ComponentCamera*)comp;
				}
				break;
			}
		}

		for (GameObject* child : go->children)
		{
			goStack.push(child);
		}
		if (go->transform != nullptr)
		{
			go->transform->UpdateTransform();
		}
	}
}

void ModuleScene::RestoreLastPhoto()
{
	if (App->scene->scenePhotos.size() > 0)
	{
		TakePhoto(scenePhotosUndoed);
		ClearScene();
		RestorePhoto(scenePhotos.back());	
		scenePhotos.pop_back();
	}
}

void ModuleScene::Redo()
{
	if (scenePhotosUndoed.size() > 0)
	{
		TakePhoto(scenePhotos);
		ClearScene();
		RestorePhoto(scenePhotosUndoed.back());
		scenePhotosUndoed.pop_back();
	}
}

void ModuleScene::LoadScene(const char* scene, const char* scenePath)
{
	ClearScene();
	if (AddScene(scene, scenePath))
	{
		path = scenePath;
		name = scene;
	}
	App->spacePartitioning->kDTree.Calculate();
	scenePhotos.clear();
}

bool ModuleScene::AddScene(const char* scene, const char* path)
{
	char* data = nullptr;
	std::string file(path);
	file += scene;
	file += JSONEXT;

	if (App->fsystem->Load(file.c_str(), &data) == 0)
	{
		RELEASE_ARRAY(data);
		return false;
	}

	JSON *json = new JSON(data);
	JSON_value* gameobjectsJSON = json->GetValue("GameObjects");
	std::map<unsigned, GameObject*> gameobjectsMap; //Necessary to assign parent-child efficiently
	gameobjectsMap.insert(std::pair<unsigned, GameObject*>(canvas->UUID, canvas));

	for (unsigned i = 0; i<gameobjectsJSON->Size(); i++)
	{		
		JSON_value* gameobjectJSON = gameobjectsJSON->GetValue(i);
		GameObject *gameobject = new GameObject();
		gameobject->Load(gameobjectJSON);
		if (gameobject->UUID != 1)
		{
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
	}

	RELEASE_ARRAY(data);
	RELEASE(json);
	return true;
}

void ModuleScene::ClearScene()
{
	CleanUp();
	camera_notfound_texture = (ResourceTexture*)App->resManager->Get(NOCAMERA);
	name.clear();	
	staticGOs.clear();
	dynamicGOs.clear();
	staticFilteredGOs.clear();
	dynamicFilteredGOs.clear();
	selection.clear();
	LOG("Reset volumetric AABBTree");
	App->spacePartitioning->aabbTree.Reset();
	LOG("Reset lighting AABBTree");
	App->spacePartitioning->aabbTreeLighting.Reset();
	App->spacePartitioning->kDTree.Calculate();
	canvas = new GameObject("Canvas", 1);
	root->InsertChild(canvas);
}

void ModuleScene::Select(GameObject * gameobject)
{
	if (App->editor->materialEditor->material != nullptr)
	{
		App->editor->materialEditor->Save();
		App->editor->materialEditor->open = false;
	}
	if (App->input->IsKeyPressed(SDLK_LCTRL))
	{
		std::list<GameObject*>::iterator it = std::find(selection.begin(), selection.end(), gameobject);
		if (it == selection.end())
		{
			selection.push_back(gameobject);
			selected = gameobject;
			selected->isSelected = true;
			App->editor->ShowInspector();
		}
		else
		{
			(*it)->isSelected = false;
			selection.erase(it);
		}		
	}
	else
	{
		for (GameObject* go : App->scene->selection)
		{
			go->isSelected = false;
		}
		selection.clear();
		selection.push_back(gameobject);
		selected = gameobject;
		selected->isSelected = true;
		App->editor->ShowInspector();
	}
	
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
	std::list<std::pair<float, GameObject*>> GOs = GetStaticIntersections(line);
	std::list<std::pair<float, GameObject*>> dGOs = GetDynamicIntersections(line);
	GOs.merge(dGOs);

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

void ModuleScene::GetStaticGlobalAABB(AABB & aabb, std::vector<GameObject*>& bucket, unsigned int & bucketOccupation)
{
	aabb.SetNegativeInfinity();
	for (GameObject* go : staticGOs)
	{
		aabb.Enclose(go->bbox);
		bucket[++bucketOccupation] = go;
	}
}


unsigned ModuleScene::GetNewUID()
{
	return uuid_rng();
}

std::list<ComponentLight*> ModuleScene::GetClosestLights(LightType type, math::float3 position) const
{
	std::map<float, ComponentLight*> lightmap;
	for (const auto& light : lights)
	{
		if (light->lightType == type && light->enabled && light->gameobject->transform != nullptr)
		{
			float distance = light->gameobject->transform->GetPosition().Distance(position);
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
	std::list<std::pair<float, GameObject*>> gos; 
	std::unordered_set<GameObject*> intersections;
	App->spacePartitioning->aabbTree.GetIntersections(line, intersections);
	for (const auto &go : intersections)
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

std::list<std::pair<float, GameObject*>> ModuleScene::GetStaticIntersections(const LineSegment & line)
{
	std::list<std::pair<float, GameObject*>> gos;
	std::unordered_set<GameObject*> intersections;
	App->spacePartitioning->kDTree.GetIntersections(line, intersections);
	for (const auto &go : intersections)
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
