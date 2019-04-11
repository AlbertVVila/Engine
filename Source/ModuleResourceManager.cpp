#include "ModuleResourceManager.h"

#include "Globals.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleProgram.h"
#include "ModuleRender.h"
#include "ModuleScene.h"
#include "ModuleFileSystem.h"

#include "Resource.h"
#include "ResourceTexture.h"
#include "ResourceModel.h"
#include "ResourceMesh.h"
#include "ResourceMaterial.h"
#include "ResourceSkybox.h"
#include "ResourceScene.h"

#include "FileImporter.h"

#include "HashString.h"
#include <algorithm>

bool sortByNameAscending(const std::string a, std::string b) { return a < b; };

ModuleResourceManager::ModuleResourceManager()
{
}


ModuleResourceManager::~ModuleResourceManager()
{
}

bool ModuleResourceManager::Init(JSON * config)
{
	LoadEngineResources();
	return true;
}

void ModuleResourceManager::LoadEngineResources()
{
	std::set<std::string> files;
	App->fsystem->ListFiles(IMPORTED_RESOURCES, files);
	for each (std::string file in files)
	{
		Resource* res = CreateNewResource(TYPE::TEXTURE);
		res->SetExportedFile((file + TEXTUREEXT).c_str());
		std::string filePath(IMPORTED_RESOURCES);
		res->SetFile((filePath + file).c_str());
		res->SetUsedByEngine(true);
	}
}

Shader* ModuleResourceManager::GetProgram(std::string filename) const
{
	std::map<std::string, std::pair<unsigned, Shader*>>::const_iterator it = shaderResources.find(filename);
	if (it != shaderResources.end())
	{
		return it->second.second;
	}
	return nullptr;
}

std::list<Shader*> ModuleResourceManager::GetAllPrograms() const
{
	std::list<Shader*> programlist;
	for (const auto & resource : shaderResources)
	{
		programlist.push_back(resource.second.second);
	}
	return programlist;
}

void ModuleResourceManager::AddProgram(Shader* shader)
{
	std::map<std::string, std::pair<unsigned, Shader*>>::iterator it = shaderResources.find(shader->file);
	if (it != shaderResources.end())
	{
		it->second.first++;
	}
	else
	{
		shaderResources.insert(std::pair<std::string, std::pair<unsigned, Shader*>>
			(shader->file, std::pair<unsigned, Shader*>(1, shader)));
		App->renderer->AddBlockUniforms(*shader); 
	}
}

void ModuleResourceManager::DeleteProgram(std::string filename)
{
	std::map<std::string, std::pair<unsigned, Shader*>>::iterator it = shaderResources.find(filename);
	if (it != shaderResources.end())
	{
		if (it->second.first > 1)
		{
			it->second.first--;
		}
		else
		{
			RELEASE(it->second.second);
			shaderResources.erase(it);
		}
	}
}

unsigned ModuleResourceManager::FindByFileInAssets(const char* fileInAssets) const
{
	for (std::map<unsigned, Resource*>::const_iterator it = resources.begin(); it != resources.end(); ++it)
	{
		if (strcmp(it->second->GetFile(), fileInAssets) == 0)
		{
			return it->first;
		}
	}
	return 0;
}

unsigned ModuleResourceManager::FindByFileInAssetsExcludingType(const char* fileInAssets, TYPE type) const
{
	for (std::map<unsigned, Resource*>::const_iterator it = resources.begin(); it != resources.end(); ++it)
	{
		if (strcmp(it->second->GetFile(), fileInAssets) == 0 && it->second->GetType() != type)
		{
			return it->first;
		}
	}
	return 0;
}

unsigned ModuleResourceManager::FindByExportedFile(const char* exportedFile) const
{
	for (std::map<unsigned, Resource*>::const_iterator it = resources.begin(); it != resources.end(); ++it)
	{
		std::string firstName(it->second->GetExportedFile());
		std::string secondName(exportedFile);
		if (firstName == secondName)
			return it->first;
		else
		{
			for (auto & c1 : firstName) c1 = toupper(c1);
			for (auto & c2 : secondName) c2 = toupper(c2);
			if (firstName == secondName)
				return it->first;
		}
	}
	return 0;
}

unsigned ModuleResourceManager::FindByExportedFile(const char* exportedFile, TYPE type) const
{
	for (std::map<unsigned, Resource*>::const_iterator it = resources.begin(); it != resources.end(); ++it)
	{
		if (it->second->GetType() == type)
		{
			std::string firstName(it->second->GetExportedFile());
			std::string secondName(exportedFile);
			if (firstName == secondName)
				return it->first;
			else
			{
				for (auto & c1 : firstName) c1 = toupper(c1);
				for (auto & c2 : secondName) c2 = toupper(c2);
				if (firstName == secondName)
					return it->first;
			}
		}
	}
	return 0;
}

bool ModuleResourceManager::ImportFile(const char* newFileInAssets, const char* filePath, TYPE type)
{
	bool success = false; 

	std::string assetPath(filePath);
	assetPath += newFileInAssets;

	// Check if the file was already imported (Mesh is excluded because has same file as model)
	unsigned uid = FindByFileInAssetsExcludingType(assetPath.c_str(), TYPE::MESH);
	if (uid != 0)
	{
		// Avoid reimporting meshes (only model can reimport them)
		return ReImportFile(GetWithoutLoad(uid), filePath, type);
	}

	Resource* resource = CreateNewResource(type);

	// Save file to import on Resource file variable
	resource->SetFile(assetPath.c_str());

	// Get filename to save exported file
	std::string exportedFile(App->fsystem->RemoveExtension(newFileInAssets));

	// If a .meta file is found import it with the configuration saved
	resource->LoadConfigFromMeta();

	switch (type) 
	{
	case TYPE::TEXTURE:		success = App->textures->ImportImage(newFileInAssets, filePath,	(ResourceTexture*)resource);	exportedFile += TEXTUREEXT;		break;
	case TYPE::MODEL:		success = App->fsystem->importer.ImportFBX(newFileInAssets, filePath, (ResourceModel*)resource);exportedFile += FBXEXTENSION;	break;
	/*case TYPE::MESH:		success = App->fsystem->importer.ImportFBX(newFileInAssets, filePath, (ResourceMesh*)resource);	exportedFile += MESHEXTENSION;	break;
	case TYPE::AUDIO:		success = App->audio->Import(newFileInAssets, written_file);									exportedFile += AUDIOEXTENSION;	break;*/
	case TYPE::SCENE:		success = App->fsystem->Copy(filePath, IMPORTED_SCENES, newFileInAssets);						exportedFile += SCENEEXTENSION;	break;
	case TYPE::MATERIAL:	success = App->fsystem->Copy(filePath, IMPORTED_MATERIALS, newFileInAssets);					exportedFile += MATERIALEXT;	break;
	}

	// If export was successful, create a new resource
	if (success) 
	{ 
		resource->SaveMetafile(assetPath.c_str());
		resource->SetExportedFile(exportedFile.c_str());
		LOG("%s imported.", resource->GetFile());
	}
	else
	{
		RELEASE(resource);
	}
	return success;
}

bool ModuleResourceManager::ReImportFile(Resource* resource, const char* filePath, TYPE type)
{
	bool success = false;

	std::string file = resource->GetExportedFile();

	switch (type)
	{
	case TYPE::TEXTURE:		success = App->textures->ImportImage(file.c_str(), filePath, (ResourceTexture*)resource);		break;
	case TYPE::MODEL:		success = App->fsystem->importer.ImportFBX(file.c_str(), filePath, (ResourceModel*)resource);	break;
	/*case TYPE::MESH:		success = App->fsystem->importer.ImportFBX(file.c_str(), filePath, (ResourceMesh*)resource);	break;
	case TYPE::AUDIO:		success = App->audio->Import(newFileInAssets, written_file);									break;*/
	case TYPE::MATERIAL:	success = App->fsystem->Copy(filePath, IMPORTED_MATERIALS, file.c_str());						break;
	case TYPE::SCENE:		success = App->fsystem->Copy(filePath, IMPORTED_SCENES, file.c_str());							break;
	}

	// If export was successful, update resource
	if (success)
	{
		std::string assetPath(filePath);
		assetPath += file;
		resource->SaveMetafile(assetPath.c_str());

		// Reload in memory
		if (resource->IsLoadedToMemory())
		{
			unsigned references = resource->GetReferences();
			resource->DeleteFromMemory();
			resource->SetReferences(references);
		}

		LOG("%s reimported.", resource->GetFile());
	}
	else
	{
		LOG("Error: %s failed on reimport.", resource->GetFile());
	}
	return success;
}

Resource* ModuleResourceManager::CreateNewResource(TYPE type, unsigned forceUid)
{
	Resource* resource = nullptr;
	unsigned uid = (forceUid == 0) ? GenerateNewUID() : forceUid;

	switch (type) 
	{
	case TYPE::TEXTURE: resource = (Resource*) new ResourceTexture(uid); break;
	case TYPE::MODEL:	resource = (Resource*) new ResourceModel(uid); break;
	case TYPE::MESH:	resource = (Resource*) new ResourceMesh(uid); break;
	case TYPE::SCENE:	resource = (Resource*) new ResourceScene(uid); break;
	/*case TYPE::AUDIO:	resource = (Resource*) new ResourceAudio(uid); break;
	case TYPE::ANIMATION: resource = (Resource*) new ResourceAnimation(uid); break;*/
	case TYPE::MATERIAL: resource = (Resource*) new ResourceMaterial(uid); break;
	case TYPE::SKYBOX: resource = (Resource*) new ResourceSkybox(uid); break;
	}

	if (resource != nullptr)
		resources[uid] = resource;

	return resource;
}

unsigned ModuleResourceManager::GenerateNewUID()
{
	return App->scene->GetNewUID();
}

Resource* ModuleResourceManager::Get(unsigned uid) const
{
	std::map<unsigned, Resource*>::const_iterator it = resources.find(uid);
	if (it == resources.end())
		return nullptr;

	Resource* resource = it->second;
	// Check if is already loaded in memory
	if (!resource->IsLoadedToMemory())
	{
		// Load in memory
		if (resource->LoadInMemory())
			return resource;
		else
			return nullptr;
	}
	else
	{
		resource->SetReferences(resource->GetReferences() + 1);
		return resource;
	}
	return nullptr;
}

Resource* ModuleResourceManager::Get(const char* exportedFile) const
{
	assert(exportedFile != NULL);

	// Look for it on the resource list
	unsigned uid = FindByExportedFile(exportedFile);
	if (uid == 0)
		return nullptr;

	// Get resource by uid
	return Get(uid);
}

Resource* ModuleResourceManager::Get(const char* exportedFile, TYPE type) const
{
	assert(exportedFile != NULL);

	// Look for it on the resource list
	unsigned uid = FindByExportedFile(exportedFile, type);
	if (uid == 0)
		return nullptr;

	// Get resource by uid
	return Get(uid);
}

ResourceMesh* ModuleResourceManager::GetMeshByName(const char* name)
{
	std::vector<std::string> resourcesList;
	for (std::map<unsigned, Resource*>::iterator it = resources.begin(); it != resources.end(); ++it)
	{
		if (it->second->GetType() == TYPE::MESH)
		{
			ResourceMesh* mesh = (ResourceMesh*)it->second;
			if (mesh->name == name)
				return (ResourceMesh*)Get(mesh->GetUID());
		}
	}

	return nullptr;
}

Resource* ModuleResourceManager::GetWithoutLoad(unsigned uid) const
{
	std::map<unsigned, Resource*>::const_iterator it = resources.find(uid);
	if (it == resources.end())
		return nullptr;

	return it->second;
}

Resource* ModuleResourceManager::GetWithoutLoad(const char* exportedFile) const
{
	assert(exportedFile != NULL);

	// Look for it on the resource list
	unsigned uid = FindByExportedFile(exportedFile);
	if (uid == 0)
		return nullptr;

	// Get resource by uid
	return GetWithoutLoad(uid);
}

Resource* ModuleResourceManager::GetWithoutLoad(const char* exportedFile, TYPE type) const
{
	assert(exportedFile != NULL);

	// Look for it on the resource list
	unsigned uid = FindByExportedFile(exportedFile,type);
	if (uid == 0)
		return nullptr;

	// Get resource by uid
	return GetWithoutLoad(uid);
}

bool ModuleResourceManager::DeleteResource(unsigned uid)
{
	std::map<unsigned, Resource*>::iterator it = resources.find(uid);
	if (it != resources.end())
	{
		if (it->second->GetReferences() > 1)
		{
			it->second->SetReferences(it->second->GetReferences() - 1);
			return true;
		}
		else if(it->second->IsLoadedToMemory())
		{
			it->second->DeleteFromMemory();
			return true;
		}
	}
	return false;
}

std::vector<Resource*> ModuleResourceManager::GetResourcesList()
{
	std::vector<Resource*> resourcesList;
	for (std::map<unsigned, Resource*>::iterator it = resources.begin(); it != resources.end(); ++it)
	{
		resourcesList.push_back(it->second);
	}
	return resourcesList;
}

std::vector<ResourceTexture*> ModuleResourceManager::GetTexturesList()
{
	std::vector<ResourceTexture*> resourcesList;
	for (std::map<unsigned, Resource*>::iterator it = resources.begin(); it != resources.end(); ++it)
	{
		if(it->second->GetType() == TYPE::TEXTURE)
			resourcesList.push_back((ResourceTexture*)it->second);
	}
	return resourcesList;
}

std::vector<ResourceMaterial*> ModuleResourceManager::GetMaterialsList()
{
	std::vector<ResourceMaterial*> resourcesList;
	for (std::map<unsigned, Resource*>::iterator it = resources.begin(); it != resources.end(); ++it)
	{
		if (it->second->GetType() == TYPE::MATERIAL)
			resourcesList.push_back((ResourceMaterial*)it->second);
	}
	return resourcesList;
}

std::vector<std::string> ModuleResourceManager::GetResourceNamesList(TYPE resourceType, bool ordered)
{
	std::vector<std::string> resourcesList;
	for (std::map<unsigned, Resource*>::iterator it = resources.begin(); it != resources.end(); ++it)
	{
		if (it->second->GetType() == resourceType)
			resourcesList.push_back(it->second->GetExportedFile());
	}

	if (ordered)	// Short by ascending order
		std::sort(resourcesList.begin(), resourcesList.end(), sortByNameAscending);

	return resourcesList;
}

std::vector<std::string> ModuleResourceManager::GetMeshesNamesList(bool ordered)
{
	std::vector<std::string> resourcesList;
	for (std::map<unsigned, Resource*>::iterator it = resources.begin(); it != resources.end(); ++it)
	{
		if (it->second->GetType() == TYPE::MESH)
		{
			ResourceMesh* mesh = (ResourceMesh*)it->second;
			resourcesList.push_back(mesh->name);
		}
	}

	if (ordered)	// Short by ascending order
		std::sort(resourcesList.begin(), resourcesList.end(), sortByNameAscending);

	return resourcesList;
}

bool ModuleResourceManager::Exists(const char* exportedFile)
{
	for (std::map<unsigned, Resource*>::iterator it = resources.begin(); it != resources.end(); ++it)
	{
		std::string firstName(it->second->GetExportedFile());
		std::string secondName(exportedFile);
		if (firstName == secondName)
			return true;
		else
		{
			for (auto & c1 : firstName) c1 = toupper(c1);
			for (auto & c2 : secondName) c2 = toupper(c2);
			if (firstName == secondName)
				return true;
		}
	}
	return false;
}

bool ModuleResourceManager::Exists(const char* exportedFile, TYPE type)
{
	for (std::map<unsigned, Resource*>::iterator it = resources.begin(); it != resources.end(); ++it)
	{
		if (it->second->GetType() == type)
		{
			std::string firstName(it->second->GetExportedFile());
			std::string secondName(exportedFile);
			if (firstName == secondName)
				return true;
			else
			{
				for (auto & c1 : firstName) c1 = toupper(c1);
				for (auto & c2 : secondName) c2 = toupper(c2);
				if (firstName == secondName)
					return true;
			}
		}
	}
	return false;
}


Resource* ModuleResourceManager::AddResource(const char* file, const char* directory, TYPE type)
{
	std::string path(directory);
	path += file;
	// Check if resource was already added
	unsigned UID = FindByFileInAssets(path.c_str());
	if ( UID == 0)
	{
		// Create new resource 
		Resource* res = CreateNewResource(type);

		// Set Exported File
		std::string exportedFile(App->fsystem->GetFilename(file));
		switch (type)
		{
		case TYPE::TEXTURE:		exportedFile += TEXTUREEXT;		break;
		case TYPE::MODEL:		exportedFile += FBXEXTENSION;	break;
		/*case TYPE::MESH:		exportedFile += MESHEXTENSION;	break;
		case TYPE::AUDIO:		exportedFile += AUDIOEXTENSION;	break;*/
		case TYPE::SCENE:		exportedFile += SCENEEXTENSION;	break;
		case TYPE::MATERIAL:	exportedFile += MATERIALEXT;	break;
		default:
			break;
		}
		res->SetExportedFile(exportedFile.c_str());
		res->SetFile(path.c_str());
		return res;
	}
	else
	{
		// Resource already exist
		return GetWithoutLoad(UID);
	}
}

Resource* ModuleResourceManager::ReplaceResource(unsigned oldResourceUID, Resource* newResource)
{
	DeleteResourceFromList(oldResourceUID);
	resources[newResource->GetUID()] = newResource;

	return nullptr;
}

void ModuleResourceManager::DeleteResourceFromList(unsigned uid)
{
	std::map<unsigned, Resource*>::const_iterator it = resources.find(uid);
	if (it != resources.end())
		resources.erase(it);
}
