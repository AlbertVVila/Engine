#include "ComponentTransform.h"
#include <assimp/scene.h> 

ComponentTransform::ComponentTransform(const aiMatrix4x4 &transform) : Component(ComponentType::Transform)
{
	AddTransform(transform);
}


ComponentTransform::~ComponentTransform()
{
}

void ComponentTransform::AddTransform(const aiMatrix4x4 & transform)
{
	aiVector3D translation;
	aiVector3D scaling;
	aiQuaternion airotation;
	transform.Decompose(scaling, airotation, translation);

	position = { translation.x, translation.y, translation.z };
	scale = { scaling.x, scaling.y, scaling.z };
	rotation = Quat(airotation.x, airotation.y, airotation.z, airotation.w);
}
