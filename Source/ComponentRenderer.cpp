#include "Application.h"

#include "ModuleResourceManager.h"
#include "ModuleFileSystem.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"

#include "GameObject.h"
#include "ComponentRenderer.h"

#include "Resource.h"
#include "ResourceMesh.h"

#include "myQuadTree.h"
#include "MaterialEditor.h"
#include "Material.h"
#include "JSON.h"

#include "imgui.h"
#include "Math/float4x4.h"

ComponentRenderer::ComponentRenderer(GameObject* gameobject) : Component(gameobject, ComponentType::Renderer)
{
	SetMaterial(DEFAULTMAT);
	gameobject->isVolumetric = true;
}

ComponentRenderer::ComponentRenderer(const ComponentRenderer& component) : Component(component)
{
	mesh = App->resManager->GetMesh(component.mesh->GetUID());
	material = component.material;
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
		if (ImGui::BeginCombo("", material->name.c_str()))
		{
			if (guiMaterials.empty())
			{
				guiMaterials = App->fsystem->ListFiles(MATERIALS, false);
			}
			for (int n = 0; n < guiMaterials.size(); n++)
			{
				bool is_selected = (material->name == guiMaterials[n]);
				if (ImGui::Selectable(guiMaterials[n].c_str(), is_selected) && material->name != guiMaterials[n])
				{
					SetMaterial(guiMaterials[n].c_str());
				}
				if (is_selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		else
		{
			guiMaterials.clear();
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
		App->resManager->DeleteResource(mesh->GetUID());
	}
	return true;
}

void ComponentRenderer::Save(JSON_value * value) const
{
	Component::Save(value);
	value->AddUint("meshUID", mesh->GetUID());
	value->AddString("materialFile", material->name.c_str());
}

void ComponentRenderer::Load(const JSON_value & value)
{
	Component::Load(value);
	unsigned uid = value.GetUint("meshUID");
	ResourceMesh* m = App->resManager->GetMesh(uid); //Look for loaded meshes
	if (m != nullptr)
	{
		mesh = m;
	}
	else //Case mesh not loaded
	{
		ResourceMesh* res = (ResourceMesh*)App->resManager->CreateNewResource(TYPE::MESH, uid);
		res->SetExportedFile(std::to_string(uid).c_str());
		m = App->resManager->GetMesh(uid); //Look for loaded meshes
		if (m != nullptr)
			m = res;
	}
	UpdateGameObject();

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

void ComponentRenderer::UpdateMesh(const char * data, unsigned uid)
{
	//mesh->SetMesh(data);
	UpdateGameObject();
}

void ComponentRenderer::UpdateGameObject()
{
	if (gameobject != nullptr)
	{
		gameobject->UpdateBBox();
		App->scene->AddToSpacePartition(gameobject);
	}
}