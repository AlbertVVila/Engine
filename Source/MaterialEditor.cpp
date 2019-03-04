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
	if (material == NULL)
	{
		return;
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

	ImGui::Spacing();

	ImGui::InputText("Name", name, 64);
	material->name = name;

	ImGui::DragFloat("kAmbient", &material->kAmbient, .005f, .0f, 1.f);
	ImGui::DragFloat("kDiffuse", &material->kDiffuse, .005f, .0f, 1.f);
	ImGui::DragFloat("kSpecular", &material->kSpecular, .005f, .0f, 1.f);
	ImGui::DragFloat("Shininess", &material->shininess, .05f, .0f, 256.f);
	ShaderSelector(currentShader);

	if (textureFiles.size() == 0)
	{
		textureFiles = App->fsystem->ListFiles(TEXTURES, false);
	}

	if (ImGui::CollapsingHeader("Diffuse"))
	{
		ImGui::PushID(&material->diffuseColor);
		ImGui::ColorEdit4("Color", (float*)&material->diffuseColor, ImGuiColorEditFlags_AlphaPreview);
		TextureSelector((unsigned)TextureType::DIFFUSE, currentDiffuse, 0);
		ImGui::Separator();
		ImGui::PopID();
	}
	if (ImGui::CollapsingHeader("Specular"))
	{
		ImGui::PushID(&material->specularColor);
		ImGui::ColorEdit3("Color", (float*)&material->specularColor);
		TextureSelector((unsigned)TextureType::SPECULAR, currentSpecular, 1);
		ImGui::Separator();
		ImGui::PopID();
	}
	if (ImGui::CollapsingHeader("Occlusion"))
	{
		ImGui::PushID(&material->textures[(unsigned)TextureType::OCCLUSION]);
		TextureSelector((unsigned)TextureType::OCCLUSION, currentOcclusion, 2);
		ImGui::Separator();
		ImGui::PopID();
	}
	if (ImGui::CollapsingHeader("Emissive"))
	{
		ImGui::ColorEdit3("Color", (float*)&material->emissiveColor);
		TextureSelector((unsigned)TextureType::EMISSIVE, currentEmissive, 3);
		ImGui::Separator();
	}
	if (ImGui::CollapsingHeader("Normal"))
	{
		TextureSelector((unsigned)TextureType::NORMAL, currentNormal, 4);
	}

	ImGui::SetCursorPosX(ImGui::GetWindowWidth()/2 - ImGui::CalcTextSize("Cancel Changes").x /2);
	if (ImGui::Button("Cancel Changes"))
	{
		material->Reset(*previous);
		SetCurrentTextures();
	}

}

void MaterialEditor::ShaderSelector(std::string & current_shader)
{
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
}

void MaterialEditor::TextureSelector(unsigned i, std::string &current_texture, int id)
{
	ImGui::PushID(id);

	if (ImGui::BeginCombo("Texture", current_texture.c_str()))
	{
		bool none_selected = (current_texture == None);
		if (ImGui::Selectable(None, none_selected))
		{
			current_texture = None;
			if (material->textures[i] != nullptr)
			{
				App->resManager->DeleteTexture(material->textures[i]->file);
				material->textures[i] = nullptr;
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

	ImGui::PopID();
}

void MaterialEditor::SetCurrentTextures()
{
	// Get shader
	if (material->shader != nullptr)
	{
		currentShader = material->shader->file;
	}
	else
	{
		currentShader = None;
	}

	// Get material textures
	Texture* diffuse_texture = material->GetTexture(TextureType::DIFFUSE);
	Texture* specular_texture = material->GetTexture(TextureType::SPECULAR);
	Texture* occlusion_texture = material->GetTexture(TextureType::OCCLUSION);
	Texture* emissive_texture = material->GetTexture(TextureType::EMISSIVE);
	Texture* normal_texture = material->GetTexture(TextureType::NORMAL);

	// Set current textures strings
	if (diffuse_texture != nullptr)		
	{ 
		currentDiffuse = diffuse_texture->file; 
	}
	else 
	{ 
		currentDiffuse = None; 
	}
	if (specular_texture != nullptr)	
	{ 
		currentSpecular = specular_texture->file;
	}
	else 
	{ 
		currentSpecular = None;
	}
	if (occlusion_texture != nullptr)	
	{ 
		currentOcclusion = occlusion_texture->file;
	}
	else 
	{ 
		currentOcclusion = None;
	}
	if (emissive_texture != nullptr)	
	{ 
		currentEmissive = emissive_texture->file; 
	}
	else 
	{ 
		currentEmissive = None; 
	}
	if (normal_texture != nullptr)		
	{ 
		currentNormal = normal_texture->file; 
	}
	else 
	{ 
		currentNormal = None; 
	}
}

void MaterialEditor::NewMaterial()
{
	if (!ImGui::IsPopupOpen(materialPopup))
	{
		ImGui::OpenPopup(materialPopup);
	}
	if (ImGui::BeginPopupModal(materialPopup, NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Create new material:");
		ImGui::Separator();
		ImGui::InputText("NewName", newName, 64);

		if (strcmp(newName, newNamePrev) != 0)
		{
			if (Exists(newName))
			{
				newMatExists = true;
			}
			else
			{
				newMatExists = false;
			}
			strcpy(newNamePrev, newName);
		}

		if (newMatExists)
		{
			std::string nameTest = newName + std::string(" already exists!");
			ImGui::Text(nameTest.c_str());
		}

		if (ImGui::Button("Save", ImVec2(120, 0)) && !newMatExists)
		{
			Material* newMaterialCreated = new Material(newName);
			newMaterialCreated->Save();
			newMaterial = false;
			RELEASE(newMaterialCreated);
			strcpy(newName, "New Material");
			strcpy(newNamePrev, "");
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			newMaterial = false;
			strcpy(newName, "New Material");
			strcpy(newNamePrev, "");
		}
		ImGui::EndPopup();
	}
}

bool MaterialEditor::Exists(const char * material)
{
	std::string materialName(material);
	return App->fsystem->Exists((MATERIALS + materialName + JSONEXT).c_str());
}

void MaterialEditor::Save()
{
	if (previous != nullptr)
	{
		if (!material->Compare(*previous))
		{
			material->Save();
		}
		RELEASE(previous);
	}
}

void MaterialEditor::CleanUp()
{
	Save();

	currentShader = None;
	currentDiffuse = None;
	currentSpecular = None;
	currentOcclusion = None;
	currentEmissive = None;

	textureFiles.clear();
	shaders.clear();
	open = false;
	material = nullptr;
	if (previous != nullptr)
	{
		RELEASE(previous);
	}

	//ImGui::CloseCurrentPopup();
}
