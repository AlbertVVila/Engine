#pragma once

#include "Mesh.h"
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/material.h>
#include <assimp/mesh.h>
#include <vector>
#include <list>
#include "GL/glew.h"
#include <string>

class Model
{
public:
	Model(const char *file);
	~Model();

	void Draw() const;
	static const char* modelFolder;

private:
	void LoadModel(const char *file);
	void GenerateMaterialData(aiMaterial * material);

	std::list<Mesh> meshes;
	std::vector<unsigned int> textures;
	const char* path;
};

