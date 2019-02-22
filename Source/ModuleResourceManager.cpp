#include "ModuleResourceManager.h"

#include "Globals.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleProgram.h"
#include "ModuleRender.h"
#include "ModuleScene.h"
#include "ModuleFileSystem.h"

#include "Material.h"
#include "Mesh.h"
#include "Resource.h"
#include "ResourceTexture.h"
#include "ResourceMesh.h"

ModuleResourceManager::ModuleResourceManager()
{
}


ModuleResourceManager::~ModuleResourceManager()
{
}

bool ModuleResourceManager::Start()
{
	//TODO: Read metafiles from Assets/ instead and import or add to resources
	std::vector<std::string> files;
	std::vector<std::string> dirs;
	App->fsystem->ListFolderContent(TEXTURES, files, dirs);
	for each (std::string file in files)
	{
		Resource* res = CreateNewResource(TYPE::TEXTURE);
		res->SetExportedFile(App->fsystem->GetFilename(file.c_str()).c_str());
		//res->exportedFile = written_file;
	}
	return true;
}

// Deprecated Texture functions
/*Texture* ModuleResourceManager::GetTexture(std::string filename) const
{
	std::map<std::string, std::pair<unsigned, Texture*>>::const_iterator it = textureResources.find(filename);
	if (it != textureResources.end())
	{
		return it->second.second;
	}
	return nullptr;
}

void ModuleResourceManager::AddTexture(Texture* texture)
{
	std::map<std::string, std::pair<unsigned, Texture*>>::iterator it = textureResources.find(texture->file);
	if (it != textureResources.end())
	{
		it->second.first++;
	}
	else
	{
		textureResources.insert(std::pair<std::string, std::pair<unsigned, Texture*>>
			(texture->file, std::pair<unsigned, Texture*>(1,texture)));
	}
}

void ModuleResourceManager::DeleteTexture(std::string filename)
{
	std::map<std::string, std::pair<unsigned, Texture*>>::iterator it = textureResources.find(filename);
	if (it != textureResources.end())
	{
		if (it->second.first > 1)
		{
			it->second.first--;
		}
		else
		{
			RELEASE(it->second.second);
			textureResources.erase(it);
		}
	}
}*/

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

Material * ModuleResourceManager::GetMaterial(std::string filename) const
{
	std::map<std::string, std::pair<unsigned, Material*>>::const_iterator it = materialResources.find(filename);
	if (it != materialResources.end())
	{
		return it->second.second;
	}
	return nullptr;
}

void ModuleResourceManager::AddMaterial(Material * material)
{
	std::map<std::string, std::pair<unsigned, Material*>>::iterator it = materialResources.find(material->name);
	if (it != materialResources.end())
	{
		it->second.first++;
	}
	else
	{
		materialResources.insert(std::pair<std::string, std::pair<unsigned, Material*>>
			(material->name, std::pair<unsigned, Material*>(1, material)));
	}
}

void ModuleResourceManager::DeleteMaterial(std::string filename)
{
	std::map<std::string, std::pair<unsigned, Material*>>::iterator it = materialResources.find(filename);
	if (it != materialResources.end())
	{
		if (it->second.first > 1)
		{
			it->second.first--;
		}
		else
		{
			RELEASE(it->second.second);
			materialResources.erase(it);
		}
	}
}

Mesh * ModuleResourceManager::GetMesh(unsigned uid) const
{
	std::map<unsigned, std::pair<unsigned, Mesh*>>::const_iterator it = meshResources.find(uid);
	if (it != meshResources.end())
	{
		return it->second.second;
	}
	return nullptr;
}

void ModuleResourceManager::AddMesh(Mesh * mesh)
{
	std::map<unsigned, std::pair<unsigned, Mesh*>>::iterator it = meshResources.find(mesh->UID);
	if (it != meshResources.end())
	{
		it->second.first++;
	}
	else
	{
		meshResources.insert(std::pair<unsigned, std::pair<unsigned, Mesh*>>
			(mesh->UID, std::pair<unsigned, Mesh*>(1, mesh)));
	}
}

void ModuleResourceManager::DeleteMesh(unsigned uid)
{
	std::map<unsigned, std::pair<unsigned, Mesh*>>::iterator it = meshResources.find(uid);
	if (it != meshResources.end())
	{
		if (it->second.first > 1)
		{
			it->second.first--;
		}
		else
		{
			RELEASE(it->second.second);
			meshResources.erase(it);
		}
	}
}

unsigned ModuleResourceManager::Find(const char* fileInAssets)
{
	for (std::map<unsigned, Resource*>::iterator it = resources.begin(); it != resources.end(); ++it)
	{
		if (strcmp(it->second->GetExportedFile(), fileInAssets) == 0)
		{
			return it->first;
		}
	}
	return 0;
}

unsigned ModuleResourceManager::ImportFile(const char* newFileInAssets, const char* filePath, TYPE type, bool force)
{
	unsigned ret = 0; 
	bool success = false; 
	std::string written_file = "";
	std::string importedFilePath(filePath);
	Resource* resource = CreateNewResource(type);

	switch (type) 
	{
	case TYPE::TEXTURE: 
		success = App->textures->ImportImage(newFileInAssets, filePath, written_file, (ResourceTexture*)resource);
		break;
	case TYPE::MESH:	

		break;
	//case TYPE::AUDIO: import_ok = App->audio->Import(newFileInAssets, written_file); break;
	//case TYPE::SCENE: import_ok = App->scene->Import(newFileInAssets, written_file); break;
	}

	// If export was successful, create a new resource
	if (success) 
	{ 
		resource->SetFile((importedFilePath + newFileInAssets).c_str());
		resource->SetExportedFile(written_file.c_str());
	}
	else
	{
		RELEASE(resource);
	}
	return ret;
}

Resource * ModuleResourceManager::CreateNewResource(TYPE type, unsigned forceUid)
{
	Resource* resource = nullptr;
	unsigned uid = (forceUid == 0) ? GenerateNewUID() : forceUid;

	switch (type) 
	{
	case TYPE::TEXTURE: resource = (Resource*) new ResourceTexture(uid); break;
	//case TYPE::MESH:	resource = (Resource*) new ResourceMesh(uid); break;
	/*case TYPE::AUDIO:	resource = (Resource*) new ResourceAudio(uid); break;
	case TYPE::SCENE:	resource = (Resource*) new ResourceScene(uid); break;
	case TYPE::BONE:	resource = (Resource*) new ResourceBone(uid); break;
	case TYPE::ANIMATION: resource = (Resource*) new ResourceAnimation(uid); break;*/
	}

	if (resource != nullptr)
		resources[uid] = resource;

	return resource;
}

unsigned ModuleResourceManager::GenerateNewUID()
{
	return App->scene->GetNewUID();
}

Resource * ModuleResourceManager::Get(unsigned uid)
{
	std::map<unsigned, Resource*>::iterator it = resources.find(uid);
	if (it != resources.end())
		return it->second;
	return nullptr;
}

bool ModuleResourceManager::DeleteTexture(unsigned uid)
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
