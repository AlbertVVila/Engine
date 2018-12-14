#ifndef __ModuleResourceManager_h__
#define __ModuleResourceManager_h__

#include "Module.h"
#include <map>

struct Texture;

class ModuleResourceManager :
	public Module
{
public:
	ModuleResourceManager();
	~ModuleResourceManager();

	Texture * GetTexture(std::string filename) const;

	void AddTexture(std::string filename, Texture * texture);

	void DeleteTexture(std::string filename);

	unsigned GetProgram(std::string filename) const;

	void AddProgram(std::string filename, unsigned program);

	void DeleteProgram(std::string filename);

private:
	std::map<std::string, std::pair<unsigned, Texture*>> textureResources; //filename , times used, texture pointer
	std::map<std::string, std::pair<unsigned, unsigned>> shaderResources; //filename , times used, shader
};

#endif __ModuleResourceManager_h__