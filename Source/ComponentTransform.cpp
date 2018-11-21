#include "ComponentTransform.h"
#include <assimp/scene.h> 
#include "imgui.h"
#include "Math/MathFunc.h"


ComponentTransform::ComponentTransform(GameObject* gameobject, const aiMatrix4x4 &transform) : Component(gameobject, ComponentType::Transform)
{
	AddTransform(transform);
}

ComponentTransform::ComponentTransform(const ComponentTransform & component) : Component(component)
{
	position = component.position;
	rotation = component.rotation;
	eulerRotation = component.eulerRotation;
	scale = component.scale;
}


ComponentTransform::~ComponentTransform()
{
}

Component * ComponentTransform::Clone()
{
	return new ComponentTransform(*this);
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
	eulerRotation = rotation.ToEulerXYZ();
	eulerRotation.x = math::RadToDeg(eulerRotation.x);
	eulerRotation.y = math::RadToDeg(eulerRotation.y);
	eulerRotation.z = math::RadToDeg(eulerRotation.z);
}

void ComponentTransform::DrawProperties()
{
	if (ImGui::CollapsingHeader("Local Transformation"))
	{
		ImGui::DragFloat3("Position", (float*)&position, 0.1f, -1000.f, 1000.f);

		ImGui::DragFloat3("Rotation", (float*)&eulerRotation, 0.5f, -180, 180.f);

		rotation =rotation.FromEulerXYZ(math::DegToRad(eulerRotation.x),
			math::DegToRad(eulerRotation.y), math::DegToRad(eulerRotation.z));

		ImGui::DragFloat3("Scale", (float*)&scale, 0.1f, 0.01f, 100.f);
		ImGui::Separator();
	}
}

