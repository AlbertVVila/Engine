#pragma once

#include <assimp/mesh.h>
#include "GL/glew.h"
#include <assert.h>
#include <vector>

class Mesh
{
public:
	int numVertices;
	int numIndices;
	int materialIndex;

	Mesh(aiMesh * mesh);
	~Mesh();

	void Draw(unsigned int shaderProgram, const std::vector<unsigned int> &textures) const;
private:
	unsigned int VAO, VBO, EBO;

};

