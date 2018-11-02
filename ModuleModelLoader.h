#ifndef __ModuleModelLoader_h__
#define __ModuleModelLoader_h__

#include "Module.h"
#include <vector>

class ModuleModelLoader :
	public Module
{
public:
	ModuleModelLoader();
	~ModuleModelLoader();

	bool Init();
	unsigned int Load(const char *path);
	void GenerateMeshData(aiMesh * mesh);
	void GenerateMaterialData(aiMaterial * material);
	void DrawModel();
	bool CleanUp();

	std::vector<int> meshVBO;
	std::vector<int> meshNumVertices;
	std::vector<int> meshNumIndices;
	std::vector<int> matIndices;
	std::vector<int> meshEBO;
	std::vector<int> MatTexture;
};

#endif __ModuleModelLoader_h__
