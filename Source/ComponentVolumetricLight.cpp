#include "ComponentVolumetricLight.h"

#include "Globals.h"
#include "Application.h"
#include "GameObject.h"

#include "imgui.h"

#pragma warning(push)
#pragma warning(disable : 4996)  
#pragma warning(disable : 4244)  
#pragma warning(disable : 4305)  
#pragma warning(disable : 4838)  
#define PAR_SHAPES_IMPLEMENTATION
#include "par_shapes.h"
#pragma warning(pop)

ComponentVolumetricLight::ComponentVolumetricLight(GameObject* gameobject) : Component(gameobject, ComponentType::VolumetricLight)
{
}

ComponentVolumetricLight::ComponentVolumetricLight(const ComponentVolumetricLight& copy) : Component(copy)
{
}

Component* ComponentVolumetricLight::Clone() const
{
	return nullptr;
}

void ComponentVolumetricLight::Update()
{
}

void ComponentVolumetricLight::DrawProperties()
{
}

void ComponentVolumetricLight::Save(JSON_value* value) const
{
}

void ComponentVolumetricLight::Load(JSON_value* value)
{
}

void ComponentVolumetricLight::Init()
{
	circle1 = par_shapes_create_disk()
}
