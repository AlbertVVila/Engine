#include "GameObject.h"

#include "Math/float4x4.h"
#include "Math/MathFunc.h"
#include "Geometry/LineSegment.h"
#include "GL/glew.h"
#include "imgui.h"

#include "Component.h"
#include "ComponentTransform.h"
#include "ComponentCamera.h"
#include "ComponentLight.h"
#include "ComponentRenderer.h"

#include "Application.h"
#include "ModuleProgram.h"
#include "ModuleEditor.h"
#include "ModuleCamera.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "ModuleTextures.h"
#include "ModuleRender.h"

#include "Material.h"
#include "Mesh.h"
#include "myQuadTree.h"
#include <stack>
#include "JSON.h"

#define MAX_NAME 64

GameObject::GameObject(const char * name, unsigned uuid) : name(name), UUID(uuid)
{
}

GameObject::GameObject(const float4x4 & transform, const char * filepath, const char * name, unsigned uuid) : name(name), UUID(uuid)
{
	this->transform =  (ComponentTransform*) CreateComponent(ComponentType::Transform);
	this->transform->AddTransform(transform);
}

GameObject::GameObject(const GameObject & gameobject)
{
	name = gameobject.name;
	UUID = App->scene->GetNewUID();
	parentUUID = gameobject.parentUUID;

	for (const auto& component: gameobject.components)
	{
		Component *componentcopy = component->Clone();
		componentcopy->gameobject = this;
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

	components.clear();
	children.clear();

	transform = nullptr;
	parent = nullptr;
}

void GameObject::DrawProperties()
{
	char *go_name = new char[MAX_NAME];
	strcpy(go_name, name.c_str());
	ImGui::InputText("Name", go_name, MAX_NAME);
	name = go_name;
	delete[] go_name;
	if (this != App->scene->root)
	{
		if (ImGui::Checkbox("Static", &isStatic))
		{
			if (isStatic && GetComponent(ComponentType::Renderer) != nullptr)
			{
				SetStaticAncestors();
				App->scene->quadtree->Insert(this);
				App->scene->dynamicGOs.erase(this);
			}
			else if (!isStatic)
			{
				App->scene->quadtree->Remove(*this); //TODO: doesn't remove on meshrenderer deletion
				App->scene->dynamicGOs.insert(this);
			}
		}
	}

	for (auto &component : components)
	{
		component->DrawProperties();
	}
}

void GameObject::DrawHierarchy(GameObject * selected)
{
	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen
		| ImGuiTreeNodeFlags_OpenOnDoubleClick | (selected == this ? ImGuiTreeNodeFlags_Selected : 0);

	ImGui::PushID(this);
	if (children.empty())
	{
		node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	}
	bool obj_open = ImGui::TreeNodeEx(this, node_flags, name.c_str());
	if (ImGui::IsItemClicked())
	{
		App->scene->Select(this);
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
			if (dropped_go != App->scene->root && dropped_go->parent != this && !dropped_go->IsParented(*this))
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
			if (ImGui::Selectable("Sphere"))
			{
				App->scene->CreateSphere("sphere0", this);
			}
			if (ImGui::Selectable("Cube"))
			{
				App->scene->CreateCube("cube0", this);
			}
			if (ImGui::Selectable("Empty GameObject"))
			{
				App->scene->CreateGameObject("Empty", this);
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
			for (const auto& copychild : copy->children)
			{
				if (copychild->GetComponent(ComponentType::Renderer) != nullptr)
				{
					if (copychild->isStatic)
					{
						App->scene->quadtree->Insert(copychild);
					}
					else
					{
						App->scene->dynamicGOs.insert(copychild);
					}
				}
			}
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
	case ComponentType::Transform:
		component = new ComponentTransform(this);
		this->transform = (ComponentTransform*)component;
		break;
	case ComponentType::Renderer:
		component = new ComponentRenderer(this);
		break;
	case ComponentType::Light:
		component = new ComponentLight(this);
		App->scene->lights.push_back((ComponentLight*)component);
		break;
	case ComponentType::Camera:
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
			component->CleanUp();
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

void GameObject::SetGlobalTransform(const float4x4 & global)
{
	if (transform != nullptr)
	{
		transform->SetLocalToWorld(global);
		float4x4 parentglobal = float4x4::identity;

		if (parent->transform != nullptr)
		{
			parentglobal = parent->GetGlobalTransform();
		}
		transform->SetWorldToLocal(parentglobal);
	}
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

void GameObject::SetLocalTransform(const float4x4 &model)
{
	if (transform == nullptr) return;
	model.Decompose(transform->position, transform->rotation, transform->scale);
}

float4x4 GameObject::GetLocalTransform() const
{
	if (transform == nullptr) return float4x4::identity;
	return float4x4::FromTRS(transform->position, transform->rotation, transform->scale);
}

void GameObject::UpdateModel(unsigned int shader) const
{
	glUniformMatrix4fv(glGetUniformLocation(shader,
		"model"), 1, GL_TRUE, &GetGlobalTransform()[0][0]);
}

void GameObject::SetLightUniforms(unsigned shader) const
{
	ComponentLight* directional = App->scene->GetDirectionalLight();
	if (directional != nullptr)
	{
		glUniform3fv(glGetUniformLocation(shader,
			"lights.directional.direction"), 1, (GLfloat*)&directional->direction);

		glUniform3fv(glGetUniformLocation(shader,
			"lights.directional.color"), 1, (GLfloat*)&directional->color);
	}
	else
	{
		float3 noDirectional = float3::zero;
		glUniform3fv(glGetUniformLocation(shader,
			"lights.directional.direction"), 1, (GLfloat*)&noDirectional);

		glUniform3fv(glGetUniformLocation(shader,
			"lights.directional.color"), 1, (GLfloat*)&noDirectional);
	}


	int i = 0;
	for (const auto & spot : App->scene->GetClosestSpotLights(transform->position))
	{
		char buffer[32];

		sprintf(buffer, "lights.spots[%d].position", i);
		glUniform3fv(glGetUniformLocation(shader,
			buffer), 1, (GLfloat*)&spot->position);

		memset(buffer, 0, 32);
		sprintf(buffer, "lights.spots[%d].direction", i);
		glUniform3fv(glGetUniformLocation(shader,
			buffer), 1, (GLfloat*)&spot->direction);

		memset(buffer, 0, 32);
		sprintf(buffer, "lights.spots[%d].color", i);
		glUniform3fv(glGetUniformLocation(shader,
			buffer), 1, (GLfloat*)&spot->color);

		memset(buffer, 0, 32);
		sprintf(buffer, "lights.spots[%d].attenuation", i);
		glUniform3fv(glGetUniformLocation(shader,
			buffer), 1, (GLfloat*)&spot->attenuation);

		memset(buffer, 0, 32);
		float innerRad = cosf(math::DegToRad(spot->inner));
		sprintf(buffer, "lights.spots[%d].inner", i);
		glUniform1fv(glGetUniformLocation(shader,
			buffer), 1, (GLfloat*)&innerRad);

		memset(buffer, 0, 32);
		float outerRad = cosf(math::DegToRad(spot->outer));
		sprintf(buffer, "lights.spots[%d].outer", i);
		glUniform1fv(glGetUniformLocation(shader,
			buffer), 1, (GLfloat*)&outerRad);

		++i;
	}
	glUniform1ui(glGetUniformLocation(shader,
		"lights.num_spots"), i);

	i = 0;
	for (const auto & point : App->scene->GetClosestPointLights(transform->position))
	{
		char buffer[32];

		sprintf(buffer, "lights.points[%d].position", i);
		glUniform3fv(glGetUniformLocation(shader,
			buffer), 1, (GLfloat*)&point->position);

		memset(buffer, 0, 32);
		sprintf(buffer, "lights.points[%d].direction", i);
		glUniform3fv(glGetUniformLocation(shader,
			buffer), 1, (GLfloat*)&point->direction);

		memset(buffer, 0, 32);
		sprintf(buffer, "lights.points[%d].color", i);
		glUniform3fv(glGetUniformLocation(shader,
			buffer), 1, (GLfloat*)&point->color);

		memset(buffer, 0, 32);
		sprintf(buffer, "lights.points[%d].attenuation", i);
		glUniform3fv(glGetUniformLocation(shader,
			buffer), 1, (GLfloat*)&point->attenuation);

		++i;
	}
	glUniform1ui(glGetUniformLocation(shader,
		"lights.num_points"), i);
}

AABB GameObject::GetBoundingBox() const
{
	AABB bbox; //Todo: Use pointers and optimize bounding box computation
	bbox.SetNegativeInfinity();
	ComponentRenderer* crenderer = (ComponentRenderer*)GetComponent(ComponentType::Renderer);
	if (crenderer != nullptr)
	{
		bbox.Enclose(crenderer->mesh->GetBoundingBox());
	}

	bbox.TransformAsAABB(GetGlobalTransform());
	for (const auto &child : children)
	{
		if(child->GetComponent(ComponentType::Renderer) != nullptr)
			bbox.Enclose(child->GetBoundingBox());
	}

	return bbox;
}

bool GameObject::MeshIntersects(const LineSegment & line, float* distance) const
{
	LineSegment localLine(line);
	localLine.Transform(GetGlobalTransform().Inverted());
	ComponentRenderer* mesh_renderer = (ComponentRenderer*)GetComponent(ComponentType::Renderer);
	if (mesh_renderer != nullptr)
	{
		if (mesh_renderer->mesh->Intersects(localLine, distance))
		{
			return true;
		}
	}
	return false;
}

void GameObject::DrawBBox() const
{ //TODO: optimize with VAO

	unsigned shader = App->program->defaultShader->id;
	glUseProgram(shader);
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
	glUniformMatrix4fv(glGetUniformLocation(shader,
		"model"), 1, GL_TRUE, &(boxtransform)[0][0]);

	float green[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
	glUniform4fv(glGetUniformLocation(shader,
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

bool GameObject::CleanUp()
{
	if (isStatic)
	{
		App->scene->quadtree->Remove(*this);
	}
	else
	{
		App->scene->dynamicGOs.erase(this);
	}

	for (auto &component : components)
	{
		component->CleanUp();
	}

	for (auto &child : children)
	{
		child->CleanUp();
	}

	transform = nullptr;
	parent = nullptr;

	return true;
}

void GameObject::Save(JSON_value *gameobjects) const
{
	if (parent != nullptr) // we don't add gameobjects without parent (ex: World)
	{
		JSON_value *gameobject = gameobjects->CreateValue();
		gameobject->AddUint("UID", UUID);
		gameobject->AddUint("ParentUID", parent->UUID);
		gameobject->AddString("Name", name.c_str());
		gameobject->AddUint("Static", isStatic);

		JSON_value *componentsJSON = gameobject->CreateValue(rapidjson::kArrayType);
		for (auto &component : components)
		{
			JSON_value *componentJSON = componentsJSON->CreateValue();
			component->Save(componentJSON);
			componentsJSON->AddValue("", componentJSON);
		}

		gameobject->AddValue("Components", componentsJSON);
		gameobjects->AddValue("", gameobject);
	}

	for (auto &child : children)
	{
		child->Save(gameobjects);
	}
}

void GameObject::Load(const JSON_value &gameobject)
{
	UUID = gameobject.GetUint("UID");
	parentUUID = gameobject.GetUint("ParentUID");
	name = gameobject.GetString("Name");
	isStatic = gameobject.GetUint("Static");

	JSON_value* componentsJSON = gameobject.GetValue("Components");
	for (unsigned i = 0; i < componentsJSON->Size(); i++)
	{
		JSON_value* componentJSON = componentsJSON->GetValue(i);
		ComponentType type = (ComponentType) componentJSON->GetUint("Type");
		Component* component = CreateComponent(type);
		component->Load(*componentJSON);
	}
}

bool GameObject::IsParented(const GameObject & gameobject)
{
	if (this == &gameobject) 
	{
		return true;
	}
	for (const auto child : children)
	{
		if (child->IsParented(gameobject))
		{
			return true;
		}
	}
	return false;
}

void GameObject::SetStaticAncestors()
{
	std::stack<GameObject*> parents;
	parents.push(parent);
	while (!parents.empty() && parents.top() != App->scene->root)
	{
		GameObject* go = parents.top();
		go->isStatic = true;

		if (go->GetComponent(ComponentType::Renderer) != nullptr)
		{
			App->scene->dynamicGOs.erase(go);
			App->scene->quadtree->Insert(go);
		}

		parents.pop();
		parents.push(go->parent);
	}
}