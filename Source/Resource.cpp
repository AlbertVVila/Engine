#include "Resource.h"
#include "JSON.h"

#include "Application.h"
#include "ModuleResourceManager.h"
#include "ModuleFileSystem.h"

Resource::Resource(unsigned uid, TYPE type): UID(uid), type(type)
{
}

Resource::Resource(const Resource& resource)
{
	UID = App->resManager->GenerateNewUID();
	file = resource.file;
	exportedFileName = resource.exportedFileName;
	loaded = resource.loaded;
	engineUsed = resource.engineUsed;
	type = resource.type;
}

Resource::~Resource()
{
	DeleteFromMemory();
}

void Resource::Copy(const Resource& resource)
{
	file = resource.file;
	exportedFileName = resource.exportedFileName;
	SetReferences(resource.loaded);
	engineUsed = resource.engineUsed;
	type = resource.type;
}

void Resource::SetReferences(unsigned references)
{
	if (references < 1 && IsLoadedToMemory())		// Delete from memory on loaded changed from > 0 to 0 
		DeleteFromMemory();
	else if (references > 0 && !IsLoadedToMemory())	// Load to memory on increase of loaded from 0 to > 0
		LoadInMemory();

	loaded = references;
}

void Resource::SaveMetafile(const char * file) const
{
}

void Resource::Rename(const char* newName)
{
	std::string ruteToFile = App->fsystem->GetFilePath(file);
	std::string fileInAssets = App->fsystem->GetFile(file);
	std::string extension = App->fsystem->GetExtension(file);

	// Rename file in Assets
	App->fsystem->Rename(ruteToFile.c_str(), fileInAssets.c_str(), newName);

	// Rename meta file in Assets
	std::string newMeta(newName);
	newMeta += extension;
	App->fsystem->Rename(ruteToFile.c_str(), (fileInAssets + ".meta").c_str(), newMeta.c_str());

	// Update file variable
	file = ruteToFile + newName + extension;

	// Rename of file in Library and update of exportedFileName is called on child class
}