#include "Application.h"
#include "Globals.h"

#include "ModuleResourceManager.h"
#include "ModuleFileSystem.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
#include "ModuleTextures.h"
#include "ModuleParticles.h"

#include "GameObject.h"
#include "ComponentParticles.h"

#include "imgui.h"

#define None "None Selected"

ComponentParticles::ComponentParticles(GameObject* gameobject) : Component(gameobject, ComponentType::Particles)
{
	if (textureFiles.size() == 0)
	{
		textureFiles = App->fsystem->ListFiles(TEXTURES, false);
	}
	App->particles->AddParticleSystem(this);
}

ComponentParticles::ComponentParticles(const ComponentParticles& component) : Component(component)
{
	if (textureFiles.size() == 0)
	{
		textureFiles = App->fsystem->ListFiles(TEXTURES, false);
	}
	App->particles->AddParticleSystem(this);

}

ComponentParticles::~ComponentParticles()
{
}

Component * ComponentParticles::Clone() const
{
	return nullptr;
}

void ComponentParticles::DrawProperties()
{
	ImGui::PushID(this);
	//texture selector
	if (ImGui::BeginCombo("Texture", textureName.c_str()))
	{
		bool none_selected = (textureName == None);
		if (ImGui::Selectable(None, none_selected))
		{
			textureName = None;
			if (texture != nullptr)
			{
				App->resManager->DeleteTexture(textureName);
				texture = nullptr;
			}
		}
		if (none_selected)
			ImGui::SetItemDefaultFocus();
		for (int n = 0; n < textureFiles.size(); n++)
		{
			bool is_selected = (textureName == textureFiles[n]);
			if (ImGui::Selectable(textureFiles[n].c_str(), is_selected))
			{
				textureName = textureFiles[n].c_str();
				texture = App->textures->GetTexture(textureName.c_str());
			}
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	if (texture != nullptr)
	{
		ImGui::Image((ImTextureID)texture->id, { 200,200 }, { 0,1 }, { 1,0 });
	}
	
	ImGui::InputInt("X Tiles", &xTiles);
	ImGui::InputInt("Y Tiles", &yTiles);
	ImGui::InputFloat("FPS", &fps, 1.f);
	ImGui::PopID();
}

bool ComponentParticles::CleanUp()
{
	return false;
}

void ComponentParticles::Update(float dt)
{

}

void ComponentParticles::Save(JSON_value* value) const
{
}

void ComponentParticles::Load(JSON_value* value)
{
}
