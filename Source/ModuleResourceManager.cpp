#include "ModuleResourceManager.h"

#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleProgram.h"
#include "ModuleRender.h"

#include "ComponentMaterial.h"


ModuleResourceManager::ModuleResourceManager()
{
}


ModuleResourceManager::~ModuleResourceManager() //TODO: deallocate mem on delete resource
{
}


Texture* ModuleResourceManager::GetTexture(std::string filename) const //TODO: use Get to retrieve or load if not found
{
	std::map<std::string, std::pair<unsigned, Texture*>>::const_iterator it = textureResources.find(filename);
	if (it != textureResources.end())
	{
		return it->second.second;
	}
	return nullptr;
}

void ModuleResourceManager::AddTexture(std::string filename, Texture* texture)
{
	std::map<std::string, std::pair<unsigned, Texture*>>::iterator it = textureResources.find(filename);
	if (it != textureResources.end())
	{
		it->second.first++;
	}
	else
	{
		textureResources.insert(std::pair<std::string, std::pair<unsigned, Texture*>>
			(filename, std::pair<unsigned, Texture*>(1,texture)));
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
			textureResources.erase(it);
		}
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
			shaderResources.erase(it);
		}
	}
}

ComponentMaterial * ModuleResourceManager::GetMaterial(std::string filename) const
{
	std::map<std::string, std::pair<unsigned, ComponentMaterial*>>::const_iterator it = materialResources.find(filename);
	if (it != materialResources.end())
	{
		return it->second.second;
	}
	return nullptr;
}

void ModuleResourceManager::AddMaterial(std::string filename, ComponentMaterial * material)
{
	std::map<std::string, std::pair<unsigned, ComponentMaterial*>>::iterator it = materialResources.find(material->name);
	if (it != materialResources.end())
	{
		it->second.first++;
	}
	else
	{
		materialResources.insert(std::pair<std::string, std::pair<unsigned, ComponentMaterial*>>
			(material->name, std::pair<unsigned, ComponentMaterial*>(1, material)));
	}
}

void ModuleResourceManager::DeleteMaterial(std::string filename)
{
	std::map<std::string, std::pair<unsigned, ComponentMaterial*>>::iterator it = materialResources.find(filename);
	if (it != materialResources.end())
	{
		if (it->second.first > 1)
		{
			it->second.first--;
		}
		else
		{
			materialResources.erase(it);
		}
	}
}
