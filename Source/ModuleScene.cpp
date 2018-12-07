#include "ModuleScene.h"
#include "Application.h"
#include "GameObject.h"
#include "Component.h"
#include "Imgui/imgui.h"
#include "ModuleEditor.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleFileSystem.h"
#include "JSON.h"
#include "rapidjson/rapidjson.h"
#include <random>

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
	root = new GameObject("World", uuid_rng());
	return true;
}
bool ModuleScene::Start()
{
	camera_notfound_texture = App->textures->Load("nocamera.jpg");
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

void ModuleScene::SaveScene()
{
	JSON *json = new JSON();
	JSON_value *array =json->CreateValue(rapidjson::kArrayType);
	root->Save(array);
	json->AddValue("GameObjects", array);
	char* file = "scene.json";
	App->fsystem->Save(file, json->ToString().c_str(), json->Size());
}

void ModuleScene::LoadScene(const char* scene)
{
	JSON *json = new JSON(scene);
	JSON_value* gameobjects = json->GetValue("GameObjects");
	//JSON *json = new JSON();
	//JSON_value *array = json->CreateValue(rapidjson::kArrayType);
	//root->Save(array);
	//json->AddValue("GameObjects", array);
	//char* file = "scene.json";
	////
	////JSON_value *uuid = json.CreateValue();
	////uuid->AddInt("parentUUID", 156);
	////uuid->AddString("name", "lolaso");
	////json.AddValue("GameObjects", uuid);
	//App->fsystem->Save(file, json->ToString().c_str(), json->Size());
}