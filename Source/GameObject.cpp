#include "GameObject.h"
#include "ComponentTransform.h"


GameObject::GameObject()
{
	components.push_back(transform = new ComponentTransform());
}


GameObject::~GameObject()
{
}

void GameObject::Update()
{
}
