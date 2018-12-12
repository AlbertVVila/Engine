#include "ComponentLight.h"
#include "ComponentTransform.h"

#include "GameObject.h"


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

ComponentLight * ComponentLight::Clone()
{
	return new ComponentLight(*this);
}
