#ifndef __ModuleResourceManager_h__
#define __ModuleResourceManager_h__

#include "Module.h"

#include <map>
#include <list>

struct Shader;
class Material;
class Resource;
class ResourceMesh;
enum class TYPE;

class ModuleResourceManager : //TODO: Divide into subclasses for each resource type
	public Module
{
public:
	ModuleResourceManager();
	~ModuleResourceManager();

	bool Start() override;

	Shader* GetProgram(std::string filename) const;
	std::list<Shader*>GetAllPrograms() const;

	void AddProgram(Shader* shader);

	void DeleteProgram(std::string filename);

	// About to be deprecated for the new ResourceManager
	Material * GetMaterial(std::string filename) const;

	void AddMaterial(Material * material);

	void DeleteMaterial(std::string filename);

	//Mesh * GetMesh(unsigned uid) const;

	//void AddMesh(Mesh * mesh);

	//void DeleteMesh(unsigned uid);

	// New ResourceManager functions
	unsigned Find(const char* fileInAssets);
	unsigned ImportFile(const char* newFileInAssets, const char* filePath, TYPE type, bool force = false);
	unsigned GenerateNewUID();
	Resource* Get(unsigned uid);
	Resource* CreateNewResource(TYPE type, unsigned forceUid = 0);
	bool DeleteResource(unsigned uid);	// If references < 1 delete it from memory

	std::list<Resource*> GetResourcesList();

	// Meshes
	ResourceMesh* GetMesh(const char* file) const;
	ResourceMesh* GetMesh(unsigned uid) const;

private:
	std::map<unsigned, Resource*> resources;	// map<UID, pointer to resource>

	// About to be deprecated for the new ResourceManager
	std::map<std::string, std::pair<unsigned, Shader*>> shaderResources; //filename , times used, shader
	std::map<std::string, std::pair<unsigned, Material*>> materialResources; //filename , times used, material
	//std::map<unsigned, std::pair<unsigned, Mesh*>> meshResources; // uid, times used, mesh
};

#endif __ModuleResourceManager_h__