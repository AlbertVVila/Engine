#ifndef __Mesh_h__
#define __Mesh_h__
#include <assimp/mesh.h>
#include "GL/glew.h"
#include <assert.h>
#include <vector>
#include "MathGeoLib.h"
#include "Globals.h"

struct Texture;

class Mesh
{
public:
	int numIndices;
	int materialIndex;
	float3 localPosition;
	Quat localRotation;
	float3 localScale;

	std::vector<float3> vertices;

	Mesh(aiMesh * mesh, aiMatrix4x4 transform);
	~Mesh();

	void Draw(unsigned int shaderProgram, const std::vector<Texture> &textures) const;

private:
	unsigned int VAO, VBO, EBO;

};

#endif //__Mesh_h__
