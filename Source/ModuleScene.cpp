#include "ModuleScene.h"
#include "ModuleEditor.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleFileSystem.h"
#include "ModuleProgram.h"

#include "Component.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"

#include "Application.h"
#include "GameObject.h"
#include "JSON.h"

#include "Imgui/imgui.h"
#include "rapidjson/rapidjson.h"

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

ModuleScene::ModuleScene()
{
}


ModuleScene::~ModuleScene()
{
}

bool ModuleScene::Init()
{
	pcg_extras::seed_seq_from<std::random_device> seed_source;
	pcg32 rng(seed_source);
	uuid_rng = rng;
	root = new GameObject("World", 0); //Root always has uid 0
	return true;
}

bool ModuleScene::Start()
{
	camera_notfound_texture = App->textures->Load(NOCAMERA); 
	return true;
}

update_status ModuleScene::Update()
{
	root->Update();
	return UPDATE_CONTINUE;
}

void ModuleScene::Draw(const math::Frustum &frustum)
{
	root->Draw(frustum);
}

void ModuleScene::DrawHierarchy()
{
	root->DrawHierarchy(selected);
}

GameObject * ModuleScene::CreateGameObject(const float4x4 & transform, const char * filepath, const char * name, GameObject* parent)
{
	GameObject * gameobject = new GameObject(transform, filepath, name, uuid_rng());
	if (parent != nullptr)
	{
		gameobject->parent = parent;
		parent->children.push_back(gameobject);
	}
	return gameobject;
}

GameObject * ModuleScene::CreateGameObject(const char * name, GameObject* parent)
{
	GameObject * gameobject = new GameObject(name, uuid_rng());
	if (parent != nullptr)
	{
		gameobject->parent = parent;
		parent->children.push_back(gameobject);
	}
	return gameobject;
}


void ModuleScene::CreateSphere(const char * name, const float3 & pos, const Quat & rot, float size, unsigned slices, unsigned stacks, const float4 & color)
{
	par_shapes_mesh* mesh = par_shapes_create_parametric_sphere(int(slices), int(stacks));
	GameObject * gameobject = App->scene->CreateGameObject(name, App->scene->root);

	ComponentTransform* componenttransform = (ComponentTransform*)gameobject->CreateComponent(ComponentType::Transform);
	gameobject->transform = componenttransform;

	componenttransform->SetRotation(rot);
	componenttransform->SetPosition(pos); //TODO: Bounding Box not resizing correctly on rotation

	if (mesh) //TODO: abstract to multiple primitives
	{
		par_shapes_scale(mesh, size, size, size);
		char* data = nullptr;
		ComponentMesh* componentmesh = (ComponentMesh*)gameobject->CreateComponent(ComponentType::Mesh);
		componentmesh->meshUID = GetNewUID();
		unsigned size = SaveParShapesMesh(*mesh, &data);
		App->fsystem->Save((MESHES + std::to_string(componentmesh->meshUID) + MESHEXTENSION).c_str(), data, size);
		componentmesh->SetMesh(data); //TODO: we should reuse mesh files for primitives

		par_shapes_free_mesh(mesh);

		ComponentMaterial* material = (ComponentMaterial*)gameobject->CreateComponent(ComponentType::Renderer);
		material->diffuse_color = color;
	}
}

unsigned ModuleScene::SaveParShapesMesh(const par_shapes_mesh_s &mesh, char** data) //TODO: unify with importer
{
	unsigned size = 0u;
	unsigned ranges[2] = { mesh.ntriangles*3, mesh.npoints};
	size += sizeof(ranges); //numfaces + numvertices
	size += ranges[0] * 3 * sizeof(int); //indices
	size += sizeof(float)*ranges[1] * 8; //vertices + texCoords + normals //TODO: case Cube with no normals!!

	*data = new char[size];
	char *cursor = *data;

	unsigned rangeBytes = sizeof(ranges); //TODO: flag or something to indicate if mesh has texcoords, normals...
	memcpy(cursor, ranges, rangeBytes);
	cursor += rangeBytes;

	unsigned verticesBytes = sizeof(float)*mesh.npoints * 3;
	memcpy(cursor, mesh.points, verticesBytes);
	cursor += verticesBytes;

	unsigned normalsBytes = sizeof(float)*mesh.npoints * 3;
	memcpy(cursor, mesh.normals, normalsBytes);
	cursor += normalsBytes;

	unsigned tcoordsBytes = sizeof(float)*mesh.npoints * 2;
	memcpy(cursor, mesh.tcoords, tcoordsBytes);
	cursor += tcoordsBytes;

	short mask = 0;
	for (unsigned i = 0; i < mesh.ntriangles*3; i++)
	{
		memcpy(cursor, &mesh.triangles[i], sizeof(short)); 
		cursor += sizeof(short);
		memcpy(cursor, &mask, sizeof(short));
		cursor += sizeof(short); //implicitly converting to unsigned
	}

	return size;
}


void ModuleScene::SaveScene(const GameObject &rootGO, const char* filename)
{
	JSON *json = new JSON();
	JSON_value *array =json->CreateValue(rapidjson::kArrayType);
	rootGO.Save(array);
	json->AddValue("GameObjects", array);
	std::string file(filename);
	App->fsystem->Save((SCENES + file + JSONEXT).c_str(), json->ToString().c_str(), json->Size());
}

void ModuleScene::LoadScene(const char* scene)
{
	char* data = nullptr;
	std::string sceneName(scene);
	name = sceneName;
	App->fsystem->Load((SCENES + sceneName + JSONEXT).c_str(), &data);
	JSON *json = new JSON(data);
	JSON_value* gameobjectsJSON = json->GetValue("GameObjects");
	std::map<unsigned, GameObject*> gameobjectsMap; //Necessary to assign parent-child efficiently
	for (unsigned i=0; i<gameobjectsJSON->Size(); i++)
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
			gameobject->parent = App->scene->root;
			gameobject->parent->children.push_back(gameobject);
		}
	}
}

unsigned ModuleScene::GetNewUID()
{
	return uuid_rng();
}