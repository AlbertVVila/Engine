#ifndef __Mesh_h__
#define __Mesh_h__
#include "Math/float3.h"
#include "Math/Quat.h"
#include "Geometry/AABB.h"
#include <vector>
#include "Component.h"

struct Texture;

class ComponentMesh : public Component
{
public:
	ComponentMesh(GameObject* gameobject, char * mesh = nullptr); //TODO: const
	ComponentMesh(const ComponentMesh& component);

	~ComponentMesh();

	Component* Clone() override;
	void Draw(unsigned int shaderProgram, const Texture* texture) const;
	void DrawProperties() override;
	void SetMesh(char * &mesh);
	unsigned int GetMaterialIndex();

	void ComputeBBox();
	AABB GetBoundingBox() const;

	void DeleteBuffers();
	void Save(JSON_value *value) const override;
	void Load(JSON_value *value) override;

public:

	int numIndices = 0;
	std::vector<float3> vertices;
	unsigned meshUID = 0u;

private:
	unsigned materialIndex = 0u;
	unsigned VAO = 0u;
	unsigned VBO = 0u;
	unsigned EBO = 0u;
	AABB boundingBox;
};

#endif //__Mesh_h__
