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
#include "ComponentCamera.h"

#include "Application.h"
#include "ModuleProgram.h"
#include "ModuleEditor.h"
#include "ModuleCamera.h"
#include "ModuleInput.h"
#include "ModuleScene.h"

#define MAX_NAME 20

GameObject::GameObject(const char * name, unsigned uuid) : name(name), UUID(uuid)
{
}

GameObject::GameObject(const float4x4 & transform, const char * filepath, const char * name, unsigned uuid) : filepath(filepath), name(name), UUID(uuid)
{
	this->transform =  (ComponentTransform*) CreateComponent(ComponentType::Transform);
	this->transform->AddTransform(transform);
}

GameObject::GameObject(const GameObject & gameobject)
{
	name = gameobject.name;
	filepath = gameobject.filepath;
	UUID = gameobject.UUID;

	for (const auto& component: gameobject.components)
	{
		Component *componentcopy = component->Clone();
		components.push_back(componentcopy);
		if (componentcopy->type == ComponentType::Transform)
		{
			transform = (ComponentTransform*)componentcopy;
		}
	}
	for (const auto& child : gameobject.children)
	{
		GameObject* childcopy = new GameObject(*child);
		childcopy->parent = this;
		children.push_back(childcopy);
	}
}


GameObject::~GameObject()
{
	for (auto &component : components)
	{
		RELEASE(component);
	}

	for (auto &child : children)
	{
		RELEASE(child);
	}

	transform = nullptr;
	parent = nullptr;
}

void GameObject::Draw(const math::Frustum& frustum)
{
	if (!frustum.Intersects(GetBoundingBox())) return; //TODO: use or compare with intersectsfaster

	for (const auto &child : children)
	{
		child->Draw(frustum);
	}

	if (transform == nullptr) return;

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

	if (texture == nullptr && material != nullptr)
	{
		shader = App->program->defaultProgram;
		glUniform4fv(glGetUniformLocation(shader,
			"Vcolor"), 1, (GLfloat*) &material->GetColor());
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
}

void GameObject::DrawProperties()
{
	char *go_name = new char[MAX_NAME];
	strcpy(go_name, name.c_str());
	ImGui::InputText("Name", go_name, MAX_NAME);
	name = go_name;
	delete[] go_name;

	for (auto &component : components)
	{
		component->DrawProperties();
	}
}

void GameObject::DrawHierarchy(GameObject * selected)
{
	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow
		| ImGuiTreeNodeFlags_OpenOnDoubleClick | (selected == this ? ImGuiTreeNodeFlags_Selected : 0);

	ImGui::PushID(this);
	if (children.empty())
	{
		node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	}
	bool obj_open = ImGui::TreeNodeEx(this, node_flags, name.c_str());
	if (ImGui::IsItemClicked())
	{
		if (App->scene->selected != nullptr)
		{
			App->scene->selected->drawBBox = false;
		}
		App->scene->selected = this;
		drawBBox = true;
	}
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
	{
		GameObject * dragged_go = this;
		ImGui::SetDragDropPayload("DragDropHierarchy", &dragged_go, sizeof(GameObject *), ImGuiCond_Once);
		ImGui::Text("%s", this->name.c_str());
		ImGui::EndDragDropSource();
	}
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragDropHierarchy"))
		{
			IM_ASSERT(payload->DataSize == sizeof(GameObject*));
			GameObject* dropped_go = (GameObject *)*(const int*)payload->Data;
			if (dropped_go->parent != this)
			{
				this->children.push_back(dropped_go);

				if (dropped_go->transform != nullptr)
				{
					dropped_go->transform->SetLocalToWorld(dropped_go->GetGlobalTransform());
				}
				dropped_go->parent->children.remove(dropped_go);
				dropped_go->parent = this;
				if (dropped_go->transform != nullptr)
				{
					dropped_go->transform->SetWorldToLocal(dropped_go->parent->GetGlobalTransform());
				}
				//TODO: reconvert transformation
			}
		}
		ImGui::EndDragDropTarget();
	}
	if (ImGui::IsItemHovered() && App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN)
	{
		ImGui::OpenPopup("gameobject_options_popup");
	}
	if (ImGui::BeginPopup("gameobject_options_popup"))
	{
		if (ImGui::BeginMenu("Create"))
		{
			const char* go_options[] = { "Empty GameObject","Cube", "Sphere" }; //TODO: Render of Cube, Sphere, Axis, Coordinates in different file?
			for (int j = 0; j < IM_ARRAYSIZE(go_options); j++)
				if (ImGui::Selectable(go_options[j]))
				{
					if (go_options[j] == "Empty GameObject")
					{
						App->scene->CreateGameObject("Empty", this);
					}
				}
			ImGui::EndMenu();
		}
		if (ImGui::Selectable("Duplicate"))
		{
			copy_flag = true;
		}
		if (ImGui::Selectable("Delete"))
		{
			delete_flag = true;
			if (selected == this)
			{
				App->scene->selected = nullptr;
			}
		}
		ImGui::EndPopup();
	}
	if (obj_open)
	{
		for (auto &child : children)
		{
			child->DrawHierarchy(selected);
		}
		if (!(node_flags & ImGuiTreeNodeFlags_NoTreePushOnOpen))
		{
			ImGui::TreePop();
		}
	}
	ImGui::PopID();
}


void GameObject::Update()
{
	for (auto& component: components)
	{
		component->Update();
	}

	for (std::list<GameObject*>::iterator it_child = children.begin(); it_child != children.end();)
	{
		(*it_child)->Update();

		if ((*it_child)->copy_flag)
		{
			(*it_child)->copy_flag = false;
			GameObject *copy = new GameObject(**it_child);
			copy->parent = this;
			this->children.push_back(copy);
		}
		if ((*it_child)->delete_flag)
		{
			(*it_child)->delete_flag = false;
			(*it_child)->CleanUp();
			delete *it_child;
			children.erase(it_child++);
		}
		else
		{
			++it_child;
		}
	}
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
	case Camera:
		component = new ComponentCamera(this);
		if (App->scene->maincamera == nullptr)
		{
			App->scene->maincamera = (ComponentCamera*)component;
		}
		break;
	default:
		break;
	}
	if (component != nullptr)
	{
		unsigned uuid = App->scene->uuid_rng();
		component->UUID = uuid;
		components.push_back(component);
	}
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

void GameObject::RemoveComponent(Component * component)
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

void GameObject::RemoveChild(GameObject* bastard)
{
	children.remove(bastard);
	RELEASE(bastard);
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

float4x4 GameObject::GetGlobalTransform() const //TODO: Move to componentTransform
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
	if (transform == nullptr) return float4x4::identity;
	return float4x4::FromTRS(transform->position, transform->rotation, transform->scale);
}

void GameObject::ModelTransform(unsigned int shader) const
{
	glUniformMatrix4fv(glGetUniformLocation(shader,
		"model"), 1, GL_TRUE, &GetGlobalTransform()[0][0]);
}

AABB GameObject::GetBoundingBox() const
{
	AABB bbox; //Todo: Use pointers and optimize bounding box computation
	bbox.SetNegativeInfinity();
	for (const auto &mesh : GetComponents(ComponentType::Mesh))
	{
		bbox.Enclose(((ComponentMesh *)mesh)->GetBoundingBox());
	}

	bbox.TransformAsAABB(GetGlobalTransform());
	for (const auto &child : children)
	{
		if(child->GetComponents(ComponentType::Mesh).size() > 0)
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

void GameObject::CleanUp()
{
	for (auto &component : components)
	{
		component->CleanUp();
	}

	for (auto &child : children)
	{
		child->CleanUp();
	}
}
