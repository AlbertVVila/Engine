#include "ModuleResourceManager.h"
#include "ModuleTextures.h"


ModuleResourceManager::ModuleResourceManager()
{
}


ModuleResourceManager::~ModuleResourceManager() //TODO: deallocate mem on delete resource
{
}


Texture* ModuleResourceManager::GetTexture(std::string filename) const
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

unsigned ModuleResourceManager::GetProgram(std::string filename) const
{
	std::map<std::string, std::pair<unsigned, unsigned>>::const_iterator it = shaderResources.find(filename);
	if (it != shaderResources.end())
	{
		return it->second.second;
	}
	return 0;
}

void ModuleResourceManager::AddProgram(std::string filename, unsigned program)
{
	std::map<std::string, std::pair<unsigned, unsigned>>::iterator it = shaderResources.find(filename);
	if (it != shaderResources.end())
	{
		it->second.first++;
	}
	else
	{
		shaderResources.insert(std::pair<std::string, std::pair<unsigned, unsigned>>
			(filename, std::pair<unsigned, unsigned>(1, program)));
	}
}

void ModuleResourceManager::DeleteProgram(std::string filename)
{
	std::map<std::string, std::pair<unsigned, unsigned>>::iterator it = shaderResources.find(filename);
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