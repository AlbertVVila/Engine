#pragma once

#include <assimp/mesh.h>
#include "GL/glew.h"
#include <assert.h>
#include <vector>
#include "MathGeoLib.h"
#include "Globals.h"

class Mesh
{
public:
	int numIndices;
	int materialIndex;
	std::vector<float3> vertices;

	Mesh(aiMesh * mesh);
	~Mesh();

	void Draw(unsigned int shaderProgram, const std::vector<unsigned int> &textures) const;
private:
	unsigned int VAO, VBO, EBO;

};

