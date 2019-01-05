#include "Application.h"

#include "ModuleProgram.h"
#include "ModuleFileSystem.h"
#include "ModuleTextures.h"

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

		std::vector<std::string> shaders = App->fsystem->ListFiles(VERTEXSHADERS, false);
		static std::string shader_current;
		if (material->shader == nullptr)
		{
			shader_current = "None Selected";
		}
		else
		{
			shader_current = material->shader->file;
		}
		if (ImGui::BeginCombo("Shader", shader_current.c_str())) // The second parameter is the label previewed before opening the combo.
		{
			for (int n = 0; n < shaders.size(); n++)
			{
				bool is_selected = (shader_current == shaders[n]);
				if (ImGui::Selectable(shaders[n].c_str(), is_selected) && shader_current != shaders[n])
				{
					shader_current = shaders[n];
					material->shader = App->program->GetProgram(shader_current.c_str());
				}
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		std::vector<std::string> textureFiles = App->fsystem->ListFiles(TEXTURES, false);
		if (ImGui::CollapsingHeader("Diffuse"))
		{
			ImGui::PushID(&material->diffuse_color);
			ImGui::ColorEdit4("Color", (float*)&material->diffuse_color, ImGuiColorEditFlags_AlphaPreview);
			static std::string diffuse_current = "None selected";
			if (ImGui::BeginCombo("Texture", diffuse_current.c_str()))
			{
				for (int n = 0; n < textureFiles.size(); n++)
				{
					bool is_selected = (diffuse_current == textureFiles[n]);
					if (ImGui::Selectable(textureFiles[n].c_str(), is_selected))
					{
						diffuse_current = textureFiles[n];
						material->textures[(unsigned)TextureType::DIFFUSE] = App->textures->GetTexture(diffuse_current.c_str());
					}
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			if (material->textures[(unsigned)TextureType::DIFFUSE] != nullptr)
			{
				ImGui::Image((ImTextureID)material->textures[(unsigned)TextureType::DIFFUSE]->id, { 200,200 }, { 0,1 }, { 1,0 });
			}
			ImGui::Separator();
			ImGui::PopID();
		}
		if (ImGui::CollapsingHeader("Specular"))
		{
			ImGui::PushID(&material->specular_color);
			ImGui::ColorEdit3("Color", (float*)&material->specular_color);
			static std::string specular_current = "None selected";
			if (ImGui::BeginCombo("Texture", specular_current.c_str()))
			{
				for (int n = 0; n < textureFiles.size(); n++)
				{
					bool is_selected = (specular_current == textureFiles[n]);
					if (ImGui::Selectable(textureFiles[n].c_str(), is_selected))
					{
						specular_current = textureFiles[n];
						material->textures[(unsigned)TextureType::SPECULAR] = App->textures->GetTexture(specular_current.c_str());
					}
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			if (material->textures[(unsigned)TextureType::SPECULAR] != nullptr)
			{
				ImGui::Image((ImTextureID)material->textures[(unsigned)TextureType::SPECULAR]->id, { 200,200 }, { 0,1 }, { 1,0 });
			}
			ImGui::Separator();
			ImGui::PopID();
		}
		if (ImGui::CollapsingHeader("Occlusion"))
		{
			ImGui::PushID(&material->textures[(unsigned)TextureType::OCCLUSION]);
			static std::string occlusion_current = "None selected";
			if (ImGui::BeginCombo("Texture", occlusion_current.c_str()))
			{
				for (int n = 0; n < textureFiles.size(); n++)
				{
					bool is_selected = (occlusion_current == textureFiles[n]);
					if (ImGui::Selectable(textureFiles[n].c_str(), is_selected))
					{
						occlusion_current = textureFiles[n];
						material->textures[(unsigned)TextureType::OCCLUSION] = App->textures->GetTexture(occlusion_current.c_str());
					}
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			if (material->textures[(unsigned)TextureType::OCCLUSION] != nullptr)
			{
				ImGui::Image((ImTextureID)material->textures[(unsigned)TextureType::OCCLUSION]->id, { 200,200 }, { 0,1 }, { 1,0 });
			}
			ImGui::Separator();
			ImGui::PopID();
		}
		if (ImGui::CollapsingHeader("Emissive"))
		{
			ImGui::ColorEdit3("Color", (float*)&material->emissive_color);
			static std::string emissive_current = "None selected";
			if (ImGui::BeginCombo("Texture", emissive_current.c_str()))
			{
				for (int n = 0; n < textureFiles.size(); n++)
				{
					bool is_selected = (emissive_current == textureFiles[n]);
					if (ImGui::Selectable(textureFiles[n].c_str(), is_selected))
					{
						emissive_current = textureFiles[n];
						material->textures[(unsigned)TextureType::EMISSIVE] = App->textures->GetTexture(emissive_current.c_str());
					}
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			if (material->textures[(unsigned)TextureType::EMISSIVE] != nullptr)
			{
				ImGui::Image((ImTextureID)material->textures[(unsigned)TextureType::EMISSIVE]->id, { 200,200 }, { 0,1 }, { 1,0 });
			}
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

void MaterialEditor::CleanUp()
{
	if (!isUsed)
	{
		material->CleanUp();
		RELEASE(material);
	}
	open = false;
	ImGui::CloseCurrentPopup();
}
