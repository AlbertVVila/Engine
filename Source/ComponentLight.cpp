#include "ComponentLight.h"

ComponentLight::ComponentLight() : Component(nullptr, ComponentType::Light)
{

}

ComponentLight::~ComponentLight()
{
}

ComponentLight * ComponentLight::Clone()
{
	return nullptr;
}
