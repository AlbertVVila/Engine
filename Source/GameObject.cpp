#include "GameObject.h"
#include "assimp/matrix4x4.h"
#include "Component.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "Application.h"
#include "ModuleProgram.h"

GameObject::GameObject()
{
	/*components.push_back(transform = new ComponentTransform());*/
}

GameObject::GameObject(const aiMatrix4x4 & transform)
{
	ComponentTransform * componentTransform =  (ComponentTransform*) CreateComponent(ComponentType::Transform);
	componentTransform->AddTransform(transform);
}


GameObject::~GameObject()
{
}

void GameObject::Draw()
{
	for (auto &component: components)
	{
		if (component->type == ComponentType::Mesh)
		{
			((ComponentMesh*)component)->Draw(App->program->defaultProgram, GetMaterialTexture());
		}
	}
}

void GameObject::Update()
{
}

Component * GameObject::CreateComponent(ComponentType type)
{
	Component* component = nullptr;

	switch (type)
	{
	case Transform:
		component = new ComponentTransform();
		break;
	case Mesh:
		component = new ComponentMesh();
		break;
	case Material:
		component = new ComponentMaterial();
		break;
	case Light:
		break;
	default:
		break;
	}
	components.push_back(component);
	return component;
}

Texture * GameObject::GetMaterialTexture() const
{
	for (auto &component : components)
	{
		if (component->type == ComponentType::Material)
		{
			return ((ComponentMaterial*)component)->GetTexture();
		}
	}
}
