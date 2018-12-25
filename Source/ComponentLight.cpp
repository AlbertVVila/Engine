#include "ComponentLight.h"
#include "ComponentTransform.h"

#include "GameObject.h"
#include "Imgui/imgui.h"

ComponentLight::ComponentLight(GameObject * gameobject) : Component(gameobject, ComponentType::Light)
{
}

ComponentLight::ComponentLight(const ComponentLight & component) : Component(component)
{
	position = component.position;
}

ComponentLight::~ComponentLight()
{
}

void ComponentLight::Update() 
{
	if (gameobject->transform == nullptr) return;
	position = gameobject->transform->position;

}

void ComponentLight::DrawProperties()
{
	if (ImGui::CollapsingHeader("Light"))
	{
		bool removed = Component::DrawComponentState();
		if (removed)
		{
			return;
		}
		ImGui::Separator();
	}
}

void ComponentLight::Load(const JSON_value & value)
{
	Component::Load(value);
	if (gameobject->transform == nullptr) return;
	position = gameobject->transform->position;
}

ComponentLight * ComponentLight::Clone() const
{
	return new ComponentLight(*this);
}
