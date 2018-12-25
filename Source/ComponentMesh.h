//#ifndef __Mesh_h__
//#define __Mesh_h__
//#include "Math/float3.h"
//#include "Math/Quat.h"
//#include "Geometry/AABB.h"
//#include <vector>
//#include "Component.h"
//
//struct Texture;
//
//class ComponentMesh : public Component
//{
//public:
//	ComponentMesh(GameObject* gameobject);
//	ComponentMesh(const ComponentMesh& component);
//
//	~ComponentMesh();
//
//	Component* Clone() const override;
//	void Draw(unsigned int shaderProgram) const;
//	void DrawProperties() override;
//	
//	unsigned int GetMaterialIndex() const;
//
//	void ComputeBBox();
//	AABB GetBoundingBox() const;
//
//	void DeleteBuffers();
//	void Save(JSON_value *value) const override;
//	void Load(const JSON_value &value) override;
//
//	unsigned GetUID() const;
//	void SetUID(unsigned UID);
//
//public:
//	Mesh* mesh = nullptr;
//
//private:
//	unsigned materialIndex = 0;
//};
//
//#endif //__Mesh_h__
