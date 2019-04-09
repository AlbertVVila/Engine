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
#include "JSON.h"

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
	ImGui::Text("Particles active %d", particles.size());
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
	xTiles = MAX(xTiles, 1);
	yTiles = MAX(yTiles, 1);
	if (ImGui::InputFloat("FPS", &fps, 1.f))
	{
		timer = 0.f;
	}
	ImGui::Separator();
	ImGui::DragFloat("Rate", &rate);
	ImGui::InputFloat2("Lifetime", &lifetime[0]);
	ImGui::InputFloat2("Speed", &speed[0]);
	ImGui::PopID();
}

bool ComponentParticles::CleanUp()
{
	unsigned nParticles = particles.size();

	for (; nParticles > 0; --nParticles)
	{
		RELEASE(particles.front());
		particles.pop();
	}

	nParticles = particlePool.size();

	for (; nParticles > 0; --nParticles)
	{
		RELEASE(particlePool.front());
		particlePool.pop();
	}

	return true;
}

void ComponentParticles::Update(float dt, const math::float3& camPos)
{
	timer += dt;
	float currentFrame = timer / (1 / fps);
	float frame;
	frameMix = modf(currentFrame, &frame);
	f1Xpos = ((int)frame) % xTiles;
	f2Xpos = (f1Xpos + 1) % xTiles;
	f1Ypos = (((int)frame) / xTiles) % yTiles;
	f2Ypos = f1Xpos < f2Xpos ? f1Ypos : f1Ypos + 1;	

	rateTimer -= dt;
	if (rateTimer <= 0.f)
	{
		int amount = MAX(dt / (1.f / rate), 1);
		for (; amount > 0; --amount)
		{
			Particle* p;
			if (particlePool.size() > 0)
			{
				p = particlePool.front();
				particlePool.pop();
			}
			else
			{
				p = new Particle();
			}
			p->position = float3(rand() % 200, 0, rand() % 200);
			if (lifetime.x != lifetime.y)
			{
				p->totalLifetime = lifetime.x + fmod(rand(), abs(lifetime.y - lifetime.x));
			}
			else
			{
				p->totalLifetime = lifetime.y;
			}
			if (speed.x != speed.y)
			{
				p->speed = speed.x + fmod(rand(), abs(speed.y - speed.x));
			}
			else
			{
				p->speed = speed.y;
			}
			p->lifeTimer = p->totalLifetime;
			particles.push(p);
		}
		rateTimer = 1.f / rate;
	}


	unsigned nParticles = particles.size();
	
	for (; nParticles > 0; --nParticles)
	{
		particles.front()->position += particles.front()->direction * particles.front()->speed * dt;
		float3 direction = (camPos - particles.front()->position);
		particles.front()->global = particles.front()->global.FromTRS(particles.front()->position, math::Quat::LookAt(float3::unitZ, direction.Normalized(), float3::unitY, float3::unitY), math::float3::one * 50);
		particles.push(particles.front());
		particles.pop();
	}
}

void ComponentParticles::Save(JSON_value* value) const
{
	Component::Save(value);
	value->AddInt("xTiles", xTiles);
	value->AddInt("yTiles", yTiles);
	value->AddFloat("fps", fps);
	value->AddString("textureName", textureName.c_str());
}

void ComponentParticles::Load(JSON_value* value)
{
	Component::Load(value);
	xTiles = value->GetInt("xTiles");
	yTiles = value->GetInt("yTiles");
	fps = value->GetFloat("fps");
	textureName = std::string(value->GetString("textureName"));
	if (textureName != "None Selected")
	{
		texture = App->textures->GetTexture(textureName.c_str());
	}
}


