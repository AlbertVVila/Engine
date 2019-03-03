#include "Application.h"
#include "ModuleRender.h"

#include "GameObject.h"

#include "ComponentBone.h"


#include "debugdraw.h"
#include "Imgui/imgui.h"
#include "Math/float4x4.h"
#include "Math/float3.h"
#include "Math/Quat.h"
#include "JSON.h"

ComponentBone::ComponentBone() : Component(nullptr, ComponentType::Bone)
{
}

ComponentBone::ComponentBone(GameObject* gameobject) : Component(gameobject, ComponentType::Bone)
{
}

ComponentBone::ComponentBone(const ComponentBone &copy) : Component(copy)
{
}

ComponentBone * ComponentBone::Clone() const
{
	return new ComponentBone(*this);
}


ComponentBone::~ComponentBone()
{
}

void ComponentBone::DrawProperties()
{
	if (ImGui::CollapsingHeader("Bone", ImGuiTreeNodeFlags_DefaultOpen))
	{
		//Draw bone info
		ImGui::Text("Bone Info");
	}
}

void ComponentBone::DrawDebug()
{
	if (App->renderer->boneDebug)
	{
		math::float3 position;
		math::float3 scale;
		math::Quat rotation;
		gameobject->GetGlobalTransform().Decompose(position, rotation, scale);

		dd::sphere(position, dd::colors::Red, 5.0f);
	}
}


void ComponentBone::Save(JSON_value* value) const
{
	Component::Save(value);
	value->AddUint("BoneUID", UID);
}

void ComponentBone::Load(const JSON_value& value)
{
	Component::Load(value);
	UID = value.GetUint("BoneUID");
}