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

void ComponentLight::Update() //TODO: serialize light
{
	if (gameobject->transform == nullptr) return;
	position = gameobject->transform->position;

}

void ComponentLight::DrawProperties()
{
	if (ImGui::CollapsingHeader("Light"))
	{
		ImGui::Separator();
	}
}

ComponentLight * ComponentLight::Clone()
{
	return new ComponentLight(*this);
}
