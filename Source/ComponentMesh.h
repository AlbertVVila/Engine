#ifndef __Mesh_h__
#define __Mesh_h__
#include "Math/float3.h"
#include "Math/Quat.h"
#include "Geometry/AABB.h"
#include <vector>
#include "Component.h"

struct par_shapes_mesh_s;
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
	void SetMesh(par_shapes_mesh_s *mesh);
	unsigned int GetMaterialIndex();

	void ComputeBBox();
	AABB GetBoundingBox() const;

	void DeleteBuffers();
	void Save(JSON_value *value) const override;
	void Load(JSON_value *value) override;

public:

	int numIndices = 0;
	const char* file = nullptr;
	std::vector<float3> vertices;

private:
	unsigned int materialIndex = 0;
	unsigned int VAO = 0;
	unsigned int VBO = 0;
	unsigned int EBO = 0;
	AABB boundingBox;
};

#endif //__Mesh_h__
