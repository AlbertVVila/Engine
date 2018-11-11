#ifndef __Mesh_h__
#define __Mesh_h__
#include <assimp/mesh.h>
#include "Math/float3.h"
#include "Math/Quat.h"
#include <vector>

struct Texture;

class Mesh
{
public:
	Mesh(aiMesh * mesh, aiMatrix4x4 transform);
	~Mesh();

	void Draw(unsigned int shaderProgram, const std::vector<Texture> &textures) const;

public:
	float3 localPosition = float3::zero;
	Quat localRotation = Quat::identity;
	float3 localScale = float3::zero;

	int numIndices = 0;
	std::vector<float3> vertices;

private:
	int materialIndex = 0;
	unsigned int VAO = 0;
	unsigned int VBO = 0;
	unsigned int EBO = 0;
};

#endif //__Mesh_h__
