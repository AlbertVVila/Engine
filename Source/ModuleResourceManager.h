#ifndef __ModuleResourceManager_h__
#define __ModuleResourceManager_h__

#include "Module.h"
#include <map>
#include <list>

struct Texture;
struct Shader;
struct Material;
struct Mesh;

class ModuleResourceManager :
	public Module
{
public:
	ModuleResourceManager();
	~ModuleResourceManager();

	Texture * GetTexture(std::string filename) const;

	void AddTexture(std::string filename, Texture * texture);

	void DeleteTexture(std::string filename);

	Shader* GetProgram(std::string filename) const;
	std::list<Shader*>GetAllPrograms() const;

	void AddProgram(Shader* shader);

	void DeleteProgram(std::string filename);

	Material * GetMaterial(std::string filename) const;

	void AddMaterial(Material * material);

	void DeleteMaterial(std::string filename);

	Mesh * GetMesh(unsigned uid) const;

	void AddMesh(Mesh * mesh);

	void DeleteMesh(unsigned uid);

private:
	std::map<std::string, std::pair<unsigned, Texture*>> textureResources; //filename , times used, texture pointer
	std::map<std::string, std::pair<unsigned, Shader*>> shaderResources; //filename , times used, shader
	std::map<std::string, std::pair<unsigned, Material*>> materialResources; //filename , times used, material
	std::map<unsigned, std::pair<unsigned, Mesh*>> meshResources; // uid, times used, mesh
};

#endif __ModuleResourceManager_h__