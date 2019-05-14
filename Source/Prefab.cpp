#include "Prefab.h"
#include "GL/glew.h"

#include "Globals.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "ComponentRenderer.h"
#include "JSON.h"

Prefab::Prefab(unsigned uid, GameObject* root) : Resource(uid, TYPE::PREFAB) //TODO: parents not prefabs?
{
	//name = root->name; //TODO: RELEASE JSON AND DATA ALL AROUND RESOURCES
	//Save(root);
}

Prefab::Prefab(const Prefab& resource) : Resource(resource)
{
}

Prefab::~Prefab()
{
	DeleteFromMemory();
}

bool Prefab::LoadInMemory()
{
	char* data = nullptr;

	unsigned ok = App->fsystem->Load((PREFABS + std::to_string(UID) + PREFABEXTENSION).c_str(), &data);

	// Load prefab
	if (ok != 0)			
	{
		Load(&data);
	}
	//TODO: RELEASE DATA iN LOAD IN MEMORY!!
	++loaded;
	return true;
}

void Prefab::DeleteFromMemory()
{
	root->CleanUp();
	RELEASE(root);
	Resource::DeleteFromMemory();
}

void Prefab::SaveMetafile(const char * file) const
{
	JSON* json = new JSON();
	JSON_value* meta = json->CreateValue();
	struct stat statFile;
	stat(file, &statFile);
	meta->AddUint("GUID", UID);
	meta->AddUint("timeCreated", statFile.st_ctime);
	std::string filepath(file);
	filepath += METAEXT;

	json->AddValue("prefab", *meta);

	App->fsystem->Save(filepath.c_str(), json->ToString().c_str(), json->Size());
	RELEASE(json);
}

void Prefab::LoadConfigFromMeta()
{
	char* data = nullptr;
	std::string metaFile(file);
	metaFile += ".meta";

	if (App->fsystem->Load(metaFile.c_str(), &data) == 0)
	{
		LOG("Warning: %s couldn't be loaded", metaFile.c_str());
		RELEASE_ARRAY(data);
		return;
	}
	JSON* json = new JSON(data);
	JSON_value* value = json->GetValue("Mesh");
	UID = value->GetUint("GUID");
	RELEASE(data);
}


void Prefab::Load(char** data)
{
	JSON *json = new JSON(*data);
	JSON_value* gameobjectsJSON = json->GetValue("GameObjects");
	std::map<unsigned, GameObject*> gameobjectsMap; //Necessary to assign parent-child efficiently

	std::list<ComponentRenderer*> renderers;

	for (unsigned i = 0; i < gameobjectsJSON->Size(); i++)
	{
		JSON_value* gameobjectJSON = gameobjectsJSON->GetValue(i);
		GameObject *gameobject = new GameObject();
		gameobject->Load(gameobjectJSON);
		if (gameobject->UUID != 1 && gameobject->UUID != 0) //TODO: Canvas and World shouldn't be a prefab
		{
			gameobjectsMap.insert(std::pair<unsigned, GameObject*>(gameobject->UUID, gameobject));
			std::map<unsigned, GameObject*>::iterator it = gameobjectsMap.find(gameobject->parentUUID);
			if (it != gameobjectsMap.end())
			{
				gameobject->parent = it->second;
				gameobject->parent->children.push_back(gameobject);
			}
		}
		else
		{
			root = gameobject;
		}

		ComponentRenderer* renderer = nullptr;
		renderer = (ComponentRenderer*)gameobject->GetComponentOld(ComponentType::Renderer);
		if (renderer != nullptr)
		{
			renderers.push_back(renderer);
		}
	}

	for (ComponentRenderer* cr : renderers)
	{
		if (cr->mesh != nullptr)
		{
			cr->LinkBones();
		}
	}

	//App->navigation->sceneLoaded(json);

	RELEASE_ARRAY(*data);
	RELEASE(json);
}

void Prefab::Save(GameObject* go) const //TODO: should also save name?
{
	JSON *json = new JSON();
	JSON_value *array = json->CreateValue(rapidjson::kArrayType);
	go->Save(array);
	json->AddValue("GameObjects", *array);

	//App->navigation->sceneSaved(json);

	App->fsystem->Save(GetFile(), json->ToString().c_str(), json->Size());
	RELEASE(json);
}

void Prefab::AddInstance(GameObject* go)
{
	instances.push_back(go);
}

void Prefab::RemoveInstance(GameObject* go)
{
	for (std::vector<GameObject*>::iterator itChild = instances.begin(); itChild != instances.end();)
	{
		if (go == *itChild)
		{
			instances.erase(itChild);
			return;
		}
	}
}

void Prefab::Update(GameObject* go)
{
	for (auto& instance : instances)
	{
		if (instance == go || !instance->isPrefabSync) continue;
		instance->UpdateToPrefab(go);
	}
	Save(go);
}

void Prefab::CheckoutPrefab()
{
	for (auto& instance : instances)
	{
		if (!instance->isPrefabSync) continue;
		instance->UpdateToPrefab(root);
	}
}
//TODO: update on start engine