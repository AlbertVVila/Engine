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

void ModuleScene::SaveScene(const GameObject &rootGO, const char* filename)
{
	JSON *json = new JSON();
	JSON_value *array =json->CreateValue(rapidjson::kArrayType);
	rootGO.Save(array);
	json->AddValue("GameObjects", array);
	std::string file(filename);
	App->fsystem->Save((file+JSONEXT).c_str(), json->ToString().c_str(), json->Size());
}

void ModuleScene::LoadScene(const char* scene)
{
	char* data = nullptr;
	std::string sceneName(scene);
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