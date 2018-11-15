#ifndef __Mesh_h__
#define __Mesh_h__
#include <assimp/mesh.h>
#include "Math/float3.h"
#include "Math/Quat.h"
#include <vector>
#include "Component.h"
struct Texture;

class ComponentMesh : public Component
{
public:
	ComponentMesh(GameObject* gameobject, aiMesh * mesh = nullptr);
	~ComponentMesh();

	void Draw(unsigned int shaderProgram, const Texture* texture) const;
	void SetMesh(const aiMesh *mesh);
	unsigned int GetMaterialIndex();

public:

	int numIndices = 0;
	std::vector<float3> vertices;

private:
	unsigned int materialIndex = 0;
	unsigned int VAO = 0;
	unsigned int VBO = 0;
	unsigned int EBO = 0;
};

#endif //__Mesh_h__
