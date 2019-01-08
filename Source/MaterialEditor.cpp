#include "Application.h"

#include "ModuleProgram.h"
#include "ModuleFileSystem.h"
#include "ModuleTextures.h"
#include "ModuleResourceManager.h"

#include "Material.h"
#include "MaterialEditor.h"
#include "imgui.h"

#define materialPopup "Material"

MaterialEditor::MaterialEditor()
{
}


MaterialEditor::~MaterialEditor()
{
}

void MaterialEditor::Draw()
{
	if (open)
	{
		if (!ImGui::IsPopupOpen(materialPopup))
		{
			ImGui::OpenPopup(materialPopup);
		}
		ImGui::SetNextWindowSizeConstraints(ImVec2(200, 200), ImVec2(500, 500));
	}
	if (ImGui::BeginPopupModal(materialPopup, NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		if (material == nullptr)
		{
			material = new Material();
		}
		char name[64] = "";
		if (!material->name.empty())
		{
			strcpy(name, material->name.c_str());
		}
		else
		{
			strcpy(name, "Unnamed");
		}
		ImGui::InputText("Name", name, 64);
		material->name = name;
		ImGui::Separator();

		ImGui::DragFloat("kAmbient", &material->kAmbient, .005f, .0f, 1.f);
		ImGui::DragFloat("kDiffuse", &material->kDiffuse, .005f, .0f, 1.f);
		ImGui::DragFloat("kSpecular", &material->kSpecular, .005f, .0f, 1.f);
		ImGui::DragFloat("Shininess", &material->shininess, .05f, .0f, 256.f);

		if (shaders.size() == 0)
		{
			shaders = App->fsystem->ListFiles(VERTEXSHADERS, false);
		}
		if (material->shader != nullptr)
		{
			current_shader = material->shader->file;
		}
		if (ImGui::BeginCombo("Shader", current_shader.c_str()))
		{
			for (int n = 0; n < shaders.size(); n++)
			{
				bool is_selected = (current_shader == shaders[n]);
				if (ImGui::Selectable(shaders[n].c_str(), is_selected) && current_shader != shaders[n])
				{
					current_shader = shaders[n];
					material->shader = App->program->GetProgram(current_shader.c_str());
				}
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		if (textureFiles.size() == 0)
		{
			textureFiles = App->fsystem->ListFiles(TEXTURES, false);
		}

		if (ImGui::CollapsingHeader("Diffuse"))
		{
			ImGui::PushID(&material->diffuse_color);
			ImGui::ColorEdit4("Color", (float*)&material->diffuse_color, ImGuiColorEditFlags_AlphaPreview);
			TextureSelector((unsigned)TextureType::DIFFUSE, current_diffuse);
			ImGui::Separator();
			ImGui::PopID();
		}
		if (ImGui::CollapsingHeader("Specular"))
		{
			ImGui::PushID(&material->specular_color);
			ImGui::ColorEdit3("Color", (float*)&material->specular_color);
			TextureSelector((unsigned)TextureType::SPECULAR, current_specular);
			ImGui::Separator();
			ImGui::PopID();
		}
		if (ImGui::CollapsingHeader("Occlusion"))
		{
			ImGui::PushID(&material->textures[(unsigned)TextureType::OCCLUSION]);
			TextureSelector((unsigned)TextureType::OCCLUSION, current_occlusion);
			ImGui::Separator();
			ImGui::PopID();
		}
		if (ImGui::CollapsingHeader("Emissive"))
		{
			ImGui::ColorEdit3("Color", (float*)&material->emissive_color);
			TextureSelector((unsigned)TextureType::EMISSIVE, current_emissive);
			ImGui::Separator();
		}
		if (ImGui::Button("Save", ImVec2(120, 0))) {
			material->Save();
			CleanUp();
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) //TODO: discard changes made in material
		{
			CleanUp();
		}
		ImGui::EndPopup();
	}

}

void MaterialEditor::TextureSelector(unsigned i, std::string &current_texture)
{
	if (ImGui::BeginCombo("Texture", current_texture.c_str()))
	{
		bool none_selected = (current_texture == None);
		if (ImGui::Selectable(None, none_selected))
		{
			current_texture = None;
			if (material->textures[i] != nullptr)
			{
				App->resManager->DeleteTexture(material->textures[i]->file);
				material->textures[i] = nullptr; //TODO: DELETE on Resource manager?
			}
		}
		if (none_selected)
			ImGui::SetItemDefaultFocus();
		for (int n = 0; n < textureFiles.size(); n++)
		{
			bool is_selected = (current_texture == textureFiles[n]);
			if (ImGui::Selectable(textureFiles[n].c_str(), is_selected))
			{
				current_texture = textureFiles[n];
				material->textures[i] = App->textures->GetTexture(current_texture.c_str());
			}
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	if (material->textures[i] != nullptr)
	{
		ImGui::Image((ImTextureID)material->textures[i]->id, { 200,200 }, { 0,1 }, { 1,0 });
	}
}
void MaterialEditor::CleanUp()
{
	if (isCreated)
	{
		material->CleanUp();
		RELEASE(material);
	}
	else
	{
		material = nullptr;
	}
	current_shader = None;
	current_diffuse = None;
	current_specular = None;
	current_occlusion = None;
	current_emissive = None;
	textureFiles.clear();
	shaders.clear();
	open = false;
	ImGui::CloseCurrentPopup();
}
