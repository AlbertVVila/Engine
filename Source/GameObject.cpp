#include "GameObject.h"

#include "assimp/matrix4x4.h"
#include "Math/float4x4.h"
#include "Geometry/AABB.h"
#include "GL/glew.h"
#include "imgui.h"

#include "Component.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"

#include "Application.h"
#include "ModuleProgram.h"
#include "ModuleEditor.h"
#include "ModuleCamera.h"


GameObject::GameObject()
{
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
	else
	{
		shader = App->program->textureProgram;
	}

	if (drawBBox)
	{
		DrawBBox();
	}

	glUseProgram(shader);
	ModelTransform(shader);

	std::vector<Component*> meshes = GetComponents(ComponentType::Mesh);
	for (auto &mesh: meshes)
	{
		if (mesh->enabled)
		{
			((ComponentMesh*)mesh)->Draw(shader, texture);
		}
	}
	glUseProgram(0);
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
		drawBBox = true;
		//App->camera->Center(this);
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

std::vector<Component *> GameObject::GetComponents(ComponentType type) const
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

Component * GameObject::GetComponent(ComponentType type) const
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

float4x4 GameObject::GetGlobalTransform() const
{
	float4x4 mytransform = GetLocalTransform();
	if (parent != nullptr)
	{
		mytransform = parent->GetGlobalTransform() * mytransform;
	}
	return mytransform;
}

float4x4 GameObject::GetLocalTransform() const
{
	return float4x4::FromTRS(transform->position, transform->rotation, transform->scale);
}

void GameObject::ModelTransform(unsigned int shader) const
{
	glUniformMatrix4fv(glGetUniformLocation(shader,
		"model"), 1, GL_TRUE, &GetGlobalTransform()[0][0]);
}

AABB GameObject::GetBoundingBox() const
{
	AABB bbox;
	bbox.SetNegativeInfinity();
	for (const auto &mesh : GetComponents(ComponentType::Mesh))
	{
		bbox.Enclose(((ComponentMesh *)mesh)->GetBoundingBox());
	}

	bbox.TransformAsAABB(GetGlobalTransform());
	for (const auto &child : children)
	{
		bbox.Enclose(child->GetBoundingBox());
	}

	return bbox;
}

void GameObject::DrawBBox() const
{ //TODO: optimize with VAO

	glUseProgram(App->program->defaultProgram);
	AABB bbox = GetBoundingBox();
	GLfloat vertices[] = {
		-0.5, -0.5, -0.5, 1.0,
		0.5, -0.5, -0.5, 1.0,
		0.5,  0.5, -0.5, 1.0,
		-0.5,  0.5, -0.5, 1.0,
		-0.5, -0.5,  0.5, 1.0,
		0.5, -0.5,  0.5, 1.0,
		0.5,  0.5,  0.5, 1.0,
		-0.5,  0.5,  0.5, 1.0,
	};
	GLuint vbo_vertices;
	glGenBuffers(1, &vbo_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLushort elements[] = {
		0, 1, 2, 3,
		4, 5, 6, 7,
		0, 4, 1, 5, 2, 6, 3, 7
	};
	GLuint ibo_elements;
	glGenBuffers(1, &ibo_elements);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	float4x4 boxtransform = float4x4::FromTRS(bbox.CenterPoint(), Quat::identity, bbox.Size());
	glUniformMatrix4fv(glGetUniformLocation(App->program->defaultProgram,
		"model"), 1, GL_TRUE, &(boxtransform)[0][0]);

	float green[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
	glUniform4fv(glGetUniformLocation(App->program->defaultProgram,
		"Vcolor"), 1, green);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,  // attribute
		4,                  // number of elements per vertex, here (x,y,z,w)
		GL_FLOAT,           // the type of each element
		GL_FALSE,           // take our values as-is
		0,                  // no extra data between each position
		0                   // offset of first element
	);

	glLineWidth(4.f);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
	glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0);
	glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid*)(4 * sizeof(GLushort)));
	glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, (GLvoid*)(8 * sizeof(GLushort)));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glLineWidth(1.f);

	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDeleteBuffers(1, &vbo_vertices);
	glDeleteBuffers(1, &ibo_elements);
	glUseProgram(0);
}

void GameObject::DisableBox()
{
	drawBBox = false;
}
