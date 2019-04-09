#include "ResourceScene.h"

#include "Globals.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleScene.h"
#include "ModuleSpacePartitioning.h"

#include "GameObject.h"

#include "JSON.h"

ResourceScene::ResourceScene(unsigned uid) : Resource(uid, TYPE::SCENE)
{
}

ResourceScene::ResourceScene(const ResourceScene& resource) : Resource(resource)
{
}

ResourceScene::~ResourceScene()
{
}

void ResourceScene::SaveMetafile(const char* file) const
{
	JSON* json = new JSON();
	JSON_value* meta = json->CreateValue();
	struct stat statFile;
	stat(file, &statFile);

	json->AddValue("Scene", *meta);

	std::string filepath(file);
	filepath += METAEXT;
	App->fsystem->Save(filepath.c_str(), json->ToString().c_str(), json->Size());
}

void ResourceScene::Rename(const char* newName)
{
	Resource::Rename(newName);

	// Rename file in Library
	std::string newExportedFile(newName);
	newExportedFile += SCENEEXTENSION;
	App->fsystem->Rename(IMPORTED_SCENES, exportedFile.c_str(), newExportedFile.c_str());

	exportedFile = newExportedFile;
}

void ResourceScene::Delete()
{
	Resource::Delete();

	// Delete file in Library
	std::string fileInLibrary(IMPORTED_SCENES);
	fileInLibrary += exportedFile;
	App->fsystem->Delete(fileInLibrary.c_str());
	DeleteFromMemory();
}

void ResourceScene::Save(const GameObject& rootGO)
{
	JSON *json = new JSON();
	JSON_value *array = json->CreateValue(rapidjson::kArrayType);
	rootGO.Save(array);
	json->AddValue("GameObjects", *array);

	App->fsystem->Save(file.c_str(), json->ToString().c_str(), json->Size());
	RELEASE(json);
}

bool ResourceScene::Load()
{
	char* data = nullptr;
	std::string sceneFile(IMPORTED_SCENES);
	sceneFile += exportedFile;

	if (App->fsystem->Load(sceneFile.c_str(), &data) == 0)
	{
		RELEASE_ARRAY(data);
		return false;
	}

	JSON *json = new JSON(data);
	JSON_value* gameobjectsJSON = json->GetValue("GameObjects");
	std::map<unsigned, GameObject*> gameobjectsMap; //Necessary to assign parent-child efficiently
	gameobjectsMap.insert(std::pair<unsigned, GameObject*>(App->scene->canvas->UUID, App->scene->canvas));

	for (unsigned i = 0; i < gameobjectsJSON->Size(); i++)
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
				gameobject->parent = App->scene->root;
				gameobject->parent->children.push_back(gameobject);
			}
		}
	}

	RELEASE_ARRAY(data);
	RELEASE(json);
	return true;
}