#include "ResourceScene.h"

#include "Globals.h"
#include "Application.h"
#include "ModuleFileSystem.h"

#include "JSON.h"

ResourceScene::ResourceScene(unsigned uid) : Resource(uid, TYPE::SCENE)
{
}

ResourceScene::ResourceScene(const ResourceScene& resource) : Resource(resource)
{
	name = resource.name;
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

	meta->AddString("name", name.c_str());
	json->AddValue("Scene", *meta);

	std::string filepath(file);
	filepath += METAEXT;
	App->fsystem->Save(filepath.c_str(), json->ToString().c_str(), json->Size());
}

void ResourceScene::Rename(const char* newName)
{
	Resource::Rename(newName);

	// Rename file in Library
	App->fsystem->Rename(IMPORTED_SCENES, (exportedFileName + SCENEEXTENSION).c_str(), newName);

	exportedFileName = newName;
}

void ResourceScene::Delete()
{
	Resource::Delete();

	// Delete file in Library
	std::string fileInLibrary(IMPORTED_SCENES);
	fileInLibrary += exportedFileName;
	fileInLibrary += SCENEEXTENSION;
	App->fsystem->Delete(fileInLibrary.c_str());
	DeleteFromMemory();
}