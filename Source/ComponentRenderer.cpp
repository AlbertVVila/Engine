#include "Application.h"

#include "ModuleResourceManager.h"
#include "ModuleFileSystem.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"

#include "GameObject.h"
#include "ComponentRenderer.h"

#include "myQuadTree.h"
#include "MaterialEditor.h"
#include "Material.h"
#include "Mesh.h"
#include "imgui.h"
#include "JSON.h"

ComponentRenderer::ComponentRenderer(GameObject * gameobject) : Component(gameobject, ComponentType::Renderer)
{
	mesh = new Mesh();
	SetMaterial(DEFAULTMAT);
}

ComponentRenderer::ComponentRenderer(const ComponentRenderer & component) : Component(component)
{
	mesh = component.mesh;
	material = component.material;

	App->resManager->AddMesh(mesh);
	App->resManager->AddMaterial(material);
}

ComponentRenderer::~ComponentRenderer()
{
	material = nullptr; //Resource Manager Deallocates resources (materials, meshes)
	mesh = nullptr;
}

Component * ComponentRenderer::Clone() const
{
	return new ComponentRenderer(*this);
}

void ComponentRenderer::DrawProperties()
{
	ImGui::PushID(this);
	if (ImGui::CollapsingHeader("Mesh Renderer", ImGuiTreeNodeFlags_DefaultOpen))
	{
		bool removed = Component::DrawComponentState();
		if (removed)
		{
			ImGui::PopID();
			return;
		}
		ImGui::Text("Num vertices : %d", mesh->numVertices);
		ImGui::Text("Num triangles : %d", mesh->numIndices / 3);
		ImGui::Separator();

		ImGui::Text("Material");
		std::vector<std::string> materials = App->fsystem->ListFiles(MATERIALS, false);
		if (ImGui::BeginCombo("", material->name.c_str()))
		{
			for (int n = 0; n < materials.size(); n++)
			{
				bool is_selected = (material->name == materials[n]);
				if (ImGui::Selectable(materials[n].c_str(), is_selected) && material->name != materials[n])
				{
					SetMaterial(materials[n].c_str());
				}
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		ImGui::SameLine();
		if (ImGui::Button("View"))
		{
			App->editor->materialEditor->material = material;
			App->editor->materialEditor->open = true; //materialpopup is only drawn once in module editor
			App->editor->materialEditor->isCreated = false; 
		}
	}
	ImGui::PopID();
}

bool ComponentRenderer::CleanUp()
{
	if (material != nullptr)
	{
		App->resManager->DeleteMaterial(material->name);
	}
	if (mesh != nullptr)
	{
		App->resManager->DeleteMesh(mesh->UID);
	}
	return true;
}

void ComponentRenderer::Save(JSON_value * value) const
{
	Component::Save(value);
	value->AddUint("meshUID", mesh->UID);
	value->AddString("materialFile", material->name.c_str());
}

void ComponentRenderer::Load(const JSON_value & value)
{
	Component::Load(value);
	unsigned uid = value.GetUint("meshUID");
	App->resManager->DeleteMesh(mesh->UID); //Delete existing old mesh
	Mesh *m = App->resManager->GetMesh(uid); //Look for loaded meshes
	if (m != nullptr)
	{
		mesh = m;
	}
	else //Case mesh not loaded
	{
		char *data = nullptr;
		App->fsystem->Load((MESHES + std::to_string(uid) + MESHEXTENSION).c_str(), &data);
		SetMesh(data, uid); //Deallocates data
	}
	App->resManager->AddMesh(mesh);

	const char* materialFile = value.GetString("materialFile");
	SetMaterial(materialFile);
}

void ComponentRenderer::SetMaterial(const char * materialfile)
{
	if (materialfile == nullptr)
	{
		materialfile = DEFAULTMAT;
	}

	if (material == nullptr || material->name != materialfile)
	{
		if (material != nullptr)
		{
			App->resManager->DeleteMaterial(material->name);
		}

		Material *mat = App->resManager->GetMaterial(materialfile);
		if (mat == nullptr)
		{
			material = new Material();
			material->Load(materialfile);
		}
		else
		{
			material = mat;
		}
		App->resManager->AddMaterial(material);
	}
	return;
}

void ComponentRenderer::SetMesh(const char* meshData, unsigned UID)
{
	mesh->SetMesh(meshData, UID);
	if (gameobject != nullptr)
	{
		gameobject->bbox = mesh->GetBoundingBox();
		if (gameobject->isStatic)
		{
			App->scene->quadtree->Insert(gameobject);
		}
		else
		{
			App->scene->dynamicGOs.insert(gameobject);
		}
	}
}