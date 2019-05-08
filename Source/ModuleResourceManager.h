#ifndef __ModuleResourceManager_h__
#define __ModuleResourceManager_h__

#include "Module.h"

#include <map>
#include <list>
#include <vector>

struct Shader;
class Resource;
class ResourceTexture;
class ResourceMaterial;
class ResourceMesh;
class ResourceAnimation;
class ResourceStateMachine;

enum class TYPE;

class ModuleResourceManager :
	public Module
{
	public:
		ModuleResourceManager();
		~ModuleResourceManager();

	bool Init(JSON* config) override;

	// Shader functions
	Shader* GetProgram(std::string filename) const;
	std::list<Shader*>GetAllPrograms() const;
	void AddProgram(Shader* shader);
	void DeleteProgram(std::string filename);

	// ResourceManager functions
	unsigned FindByFileInAssets(const char* fileInAssets) const;						// Returns UID of resource by file variable 
	unsigned FindByFileInAssetsExcludingType(const char* fileInAssets, TYPE type) const;// Returns UID of resource by file variable, excludes files of the type given
	unsigned FindByExportedFile(const char* exportedFile, TYPE type) const;				// Returns UID of resource by exportedFile and type
	unsigned FindByExportedFile(const char* exportedFile) const;						// Returns UID of resource by exportedFile
	bool ImportFile(const char* newFileInAssets, const char* filePath, TYPE type);
	bool ReImportFile(Resource* resource, const char* filePath, TYPE type);				// Imports again an already loaded resource
	unsigned GenerateNewUID();
	Resource* Get(unsigned uid) const;													// Returns the resource using UID adding one to the references count and loads it to memory if not already
	Resource* Get(const char* file) const;												// Returns the resource using exportedFileName adding one to the references count and loads it to memory if not already
	Resource* Get(const char* file, TYPE type) const;									// Returns the resource using exportedFileName and type, adding one to the references count and loads it to memory if not already
	ResourceMesh* GetMeshByName(const char* name);										// To be deprecated: returns mesh by name.
	ResourceAnimation* GetAnimationByName(const char* name);
	ResourceStateMachine* GetSMByName(const char* name);

	Resource* GetWithoutLoad(unsigned uid) const;										// Returns the resource using UID and doesn't add one to the references count neither loads it to memory
	Resource* GetWithoutLoad(const char* file) const;									// Returns the resource using exportedFileName and doesn't add one to the references count neither loads it to memory
	Resource* GetWithoutLoad(const char* file, TYPE type) const;						// Returns the resource using exportedFileName and type, also doesn't add one to the references count neither loads it to memory
	Resource* CreateNewResource(TYPE type, unsigned forceUid = 0);
	bool DeleteResource(unsigned uid);													// If references < 1 delete it from memory

	std::vector<Resource*> GetResourcesList();
	std::vector<ResourceTexture*> GetTexturesList();
	std::vector<ResourceMaterial*> GetMaterialsList();
	std::vector<ResourceAnimation*> GetAnimationsList();
	std::vector<ResourceStateMachine*> GetSMList();

	std::vector<std::string> GetResourceNamesList(TYPE resourceType, bool ordered); // Returns a vector with the exportedFileName of every Resource of the type given.	
	std::vector<std::string> GetMeshesNamesList(bool ordered);						// To be deprecated: Returns a vector with the name of every mesh orederer or not.		
	std::vector<std::string> GetAnimationsNamesList(bool ordered);
	std::vector<std::string> GetSMNamesList(bool ordered);

	bool Exists(const char* exportedFile);											// Checks if a resource with the given exported filename already exist
	bool Exists(const char* exportedFile, TYPE type);								// Checks if a resource of the given type and exported filename already exist

	void LoadEngineResources();														// Loads resources needed by the engine (Skybox, white, no camera textures...)
	Resource* AddResource(const char* file, const char* directory, TYPE type);
	Resource* ReplaceResource(unsigned oldResourceUID, Resource* newResource);	
	void DeleteResourceFromList(unsigned uid);

private:
	// Resources map (Textures, Models, Mehses, Materials, Skyboxes, Scenes...)
	std::map<unsigned, Resource*> resources;	// map<UID, pointer to resource>

	// Shaders map
	std::map<std::string, std::pair<unsigned, Shader*>> shaderResources; //filename , times used, shader
};

#endif __ModuleResourceManager_h__
