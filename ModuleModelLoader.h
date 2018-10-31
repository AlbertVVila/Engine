#ifndef __ModuleModelLoader_h__
#define __ModuleModelLoader_h__

#include "Module.h"
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/material.h>
#include <assimp/mesh.h>
class ModuleModelLoader :
	public Module
{
public:
	ModuleModelLoader();
	~ModuleModelLoader();

	bool Init();
	unsigned int Load(const char *path);
	void GenerateMeshData(aiMesh * mesh);
	void GenerateMaterialData(aiMaterial * mesh);
	void DrawModel();
	bool CleanUp();
};

#endif __ModuleModelLoader_h__
