#include "Component.h"
#include "GameObject.h"

Component::Component(GameObject* gameobject, ComponentType type)
{
	this->gameobject = gameobject;
	this->type = type;
}

Component::~Component()
{
}
