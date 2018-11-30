#include "ComponentTransform.h"
#include <assimp/scene.h> 
#include "imgui.h"
#include "Math/MathFunc.h"
#include "Math/float4x4.h"


ComponentTransform::ComponentTransform(GameObject* gameobject, const float4x4 &transform) : Component(gameobject, ComponentType::Transform)
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

void ComponentTransform::AddTransform(const float4x4 & transform)
{
	transform.Decompose(position, rotation, scale);
	RotationToEuler();
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

void ComponentTransform::SetRotation(const Quat & rot)
{
	rotation = rot;
	RotationToEuler();
}

void ComponentTransform::RotationToEuler()
{
	eulerRotation = rotation.ToEulerXYZ();
	eulerRotation.x = math::RadToDeg(eulerRotation.x);
	eulerRotation.y = math::RadToDeg(eulerRotation.y);
	eulerRotation.z = math::RadToDeg(eulerRotation.z);
}
void ComponentTransform::SetPosition(const float3 & pos)
{
	position = pos;
}

void ComponentTransform::SetLocalToWorld(const float4x4 & myGlobal)
{
	float4x4 world = myGlobal;
	world.Decompose(position, rotation, scale);
	RotationToEuler();
}



void ComponentTransform::SetWorldToLocal(const float4x4 & newparentGlobalMatrix)
{
	float4x4 world = float4x4::FromTRS(position, rotation, scale);
	float4x4 local = newparentGlobalMatrix.Inverted() * world;
	local.Decompose(position, rotation, scale);
	RotationToEuler();
}