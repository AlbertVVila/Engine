#pragma once

#include "Mesh.h"
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/material.h>
#include <assimp/mesh.h>
#include <vector>

class Model
{
public:
	Model(const char *file);
	~Model();

private:
	void loadModel(const char *file);
	void GenerateMeshData(aiMesh * mesh);
	void GenerateMaterialData(aiMaterial * material);

	std::vector<Mesh> meshes;
	const char* file;
};

