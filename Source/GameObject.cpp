#include "GameObject.h"
#include "assimp/matrix4x4.h"
#include "Math/float4x4.h"
#include "Component.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "Application.h"
#include "ModuleProgram.h"
#include "ModuleEditor.h"
#include "GL/glew.h"
#include "imgui.h"

GameObject::GameObject()
{
	/*components.push_back(transform = new ComponentTransform());*/
}

GameObject::GameObject(const aiMatrix4x4 & transform, const char * filepath, const char * name)
{
	this->filepath = filepath;
	this->name = name;
	this->transform =  (ComponentTransform*) CreateComponent(ComponentType::Transform);
	this->transform->AddTransform(transform);
}


GameObject::~GameObject()
{
}

void GameObject::Draw()
{
	ComponentMaterial* material = (ComponentMaterial*)GetComponent(ComponentType::Material);
	unsigned int shader = 0;
	Texture * texture = nullptr;

	if (material != nullptr && material->enabled)
	{
		shader = material->GetShader();
		texture = material->GetTexture();
	}
	ModelTransform(shader);

	std::vector<Component*> meshes = GetComponents(ComponentType::Mesh);
	for (auto &mesh: meshes)
	{
		if (mesh->enabled)
		{
			((ComponentMesh*)mesh)->Draw(App->program->defaultProgram, texture);
		}
	}
	//Now do it for each child
	for (const auto &child : children)
	{
		child->Draw();
	}
}

void GameObject::DrawProperties()
{
	for (auto &component : components)
	{
		component->DrawProperties();
	}
}

void GameObject::DrawHierarchy(int &obj_clicked, int i)
{
	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow
		| ImGuiTreeNodeFlags_OpenOnDoubleClick | (obj_clicked == i ? ImGuiTreeNodeFlags_Selected : 0);

	ImGui::PushID(this);
	bool obj_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, name.c_str());
	if (ImGui::IsItemClicked())
	{
		obj_clicked = i;
		App->editor->ShowInspector(this);
	}
	if (obj_open)
	{
		for (auto &child : children)
		{
			child->DrawHierarchy(obj_clicked, ++i);
		}
		ImGui::TreePop();
	}
	ImGui::PopID();
}

void GameObject::SetParent(GameObject * parent)
{
	this->parent = parent;
	if (parent != nullptr)
	{
		parent->children.push_back(this);
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
		component = new ComponentTransform(this);
		this->transform = (ComponentTransform*)component;
		break;
	case Mesh:
		component = new ComponentMesh(this);
		break;
	case Material:
		component = new ComponentMaterial(this);
		break;
	case Light:
		break;
	default:
		break;
	}
	components.push_back(component);
	return component;
}

std::vector<Component *> GameObject::GetComponents(ComponentType type)
{
	std::vector<Component *> list;
	for (auto &component : components)
	{
		if (component->type == type)
		{
			list.push_back(component);
		}
	}
	return list;
}

void GameObject::DeleteComponent(Component * component)
{
	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
	{
		if (*it == component)
		{
			components.erase(it);
			RELEASE(component);
			return;
		}
	}
}

Component * GameObject::GetComponent(ComponentType type)
{
	for (auto &component : components)
	{
		if (component->type == type)
		{
			return component;
		}
	}
	return nullptr;
}

std::string GameObject::GetFileFolder() const
{
	std::string s(filepath);
	std::size_t found = s.find_last_of("/\\");
	s = s.substr(0, found + 1);
	return s;
}

void GameObject::ModelTransform(unsigned int shader) const
{
	float4x4 model(float4x4::FromTRS(transform->position, transform->rotation, transform->scale));
	glUniformMatrix4fv(glGetUniformLocation(shader,
		"model"), 1, GL_TRUE, &model[0][0]);
}
