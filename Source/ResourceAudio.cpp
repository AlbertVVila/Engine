#include "ResourceAudio.h"

#include "Application.h"
#include "ModuleFileSystem.h"

#include "JSON.h"

ResourceAudio::ResourceAudio(unsigned uid) : Resource(uid, TYPE::AUDIO)
{
}

ResourceAudio::ResourceAudio(const ResourceAudio& resource) : Resource(resource)
{
}


ResourceAudio::~ResourceAudio()
{
	DeleteFromMemory();
}

bool ResourceAudio::LoadInMemory()
{
	if (Resource::IsLoadedToMemory())
		return false;

	bool success = false;

	// Increase references
	if (success) ++loaded;
	return success;
}

void ResourceAudio::DeleteFromMemory()
{
	Resource::DeleteFromMemory();
}

void ResourceAudio::SaveMetafile(const char * file) const
{
	std::string filepath;
	filepath.append(file);
	JSON* json = new JSON();
	JSON_value* meta = json->CreateValue();
	struct stat statFile;
	stat(filepath.c_str(), &statFile);
	meta->AddUint("GUID", UID);
	meta->AddUint("timeCreated", statFile.st_ctime);
	json->AddValue("Audio", *meta);
	filepath += METAEXT;
	App->fsystem->Save(filepath.c_str(), json->ToString().c_str(), json->Size());
}

void ResourceAudio::LoadConfigFromMeta()
{
	std::string metaFile(file);
	metaFile += ".meta";

	// Check if meta file exists
	if (!App->fsystem->Exists(metaFile.c_str()))
		return;

	char* data = nullptr;
	unsigned oldUID = GetUID();

	if (App->fsystem->Load(metaFile.c_str(), &data) == 0)
	{
		LOG("Warning: %s couldn't be loaded", metaFile.c_str());
		RELEASE_ARRAY(data);
		return;
	}
	JSON* json = new JSON(data);
	JSON_value* value = json->GetValue("Texture");

	// Make sure the UID from meta is the same
	unsigned checkUID = value->GetUint("GUID");
}
