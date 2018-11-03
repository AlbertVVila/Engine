#pragma once

#include "Mesh.h"
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/material.h>
#include <assimp/mesh.h>
#include <vector>
#include "GL/glew.h"

class Model
{
public:
	Model(const char *file);
	~Model();

	void Draw() const;
private:
	void loadModel(const char *file);
	void GenerateMaterialData(aiMaterial * material);
	
	std::vector<Mesh> meshes;
	std::vector<unsigned int> textures;
	const char* file;
};

