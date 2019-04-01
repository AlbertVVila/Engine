#include "ResourceModel.h"

#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleResourceManager.h"

#include "ResourceMesh.h"
#include "JSON.h"

ResourceModel::ResourceModel(unsigned uid) : Resource(uid, TYPE::MODEL)
{
}

ResourceModel::ResourceModel(const ResourceModel& resource) : Resource(resource)
{
	numMeshes = numMeshes;
	meshList = resource.meshList;
}


ResourceModel::~ResourceModel()
{
	DeleteFromMemory();
}

bool ResourceModel::LoadInMemory()
{
	for each(auto mesh in meshList)
	{
		mesh->LoadInMemory();
	}
	return true;
}

void ResourceModel::DeleteFromMemory()
{
	Resource::DeleteFromMemory();

	for each(auto mesh in meshList)
	{
		mesh->DeleteFromMemory();
	}
}

void ResourceModel::SaveMetafile(const char* file) const
{
	std::string filepath;
	filepath.append(file);
	JSON *json = new JSON();
	JSON_value* meta = json->CreateValue();
	struct stat statFile;
	stat(filepath.c_str(), &statFile);
	meta->AddUint("GUID", UID);
	meta->AddUint("timeCreated", statFile.st_ctime);
	meta->AddUint("NumMeshes", numMeshes);
	for (int i = 0; i < numMeshes; ++i)
	{
		meta->AddUint(("Mesh" + std::to_string(i)).c_str(), meshList[i]->GetUID());
	}
	json->AddValue("Mesh", *meta);
	filepath += METAEXT;
	App->fsystem->Save(filepath.c_str(), json->ToString().c_str(), json->Size());
}

void ResourceModel::LoadConfigFromMeta()
{
	Resource::LoadConfigFromMeta();

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
	numMeshes = value->GetUint("NumMeshes");

	std::string name = App->fsystem->GetFilename(file);
	for (int i = 0; i < numMeshes; ++i)
	{
		unsigned meshUID = value->GetUint(("Mesh" + std::to_string(i)).c_str());
		ResourceMesh* mesh = (ResourceMesh*)App->resManager->CreateNewResource(TYPE::MESH, meshUID);
		mesh->SetFile(file.c_str());
		mesh->SetExportedFile((name + std::to_string(i)).c_str());

		meshList.push_back(mesh);
	}
}

bool ResourceModel::CheckImportedMeshes()
{
	std::set<std::string> importedMeshes;
	App->fsystem->ListFiles(MESHES, importedMeshes);

	for each(auto mesh in meshList)
	{
		std::set<std::string>::iterator it = importedMeshes.find(std::to_string(mesh->GetUID()));
		if (it == importedMeshes.end())
		{
			return true;
		}
	}
	return false;
}

void ResourceModel::AddMesh(ResourceMesh* mesh)
{
	++numMeshes;
	meshList.push_back(mesh);
}