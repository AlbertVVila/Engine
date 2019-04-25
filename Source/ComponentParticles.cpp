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
#include "ResourceTexture.h"

#include "imgui.h"
#include "JSON.h"

#define None "None Selected"

ComponentParticles::ComponentParticles(GameObject* gameobject) : Component(gameobject, ComponentType::Particles)
{
	textureFiles = App->resManager->GetResourceNamesList(TYPE::TEXTURE, true);
	App->particles->AddParticleSystem(this);

}

ComponentParticles::ComponentParticles(const ComponentParticles& component) : Component(component)
{
	textureFiles = App->resManager->GetResourceNamesList(TYPE::TEXTURE, true);
	textureName = component.textureName;
	if (textureName != "None Selected")
	{
		unsigned imageUID = App->resManager->FindByExportedFile(textureName.c_str());
		texture = (ResourceTexture*)App->resManager->Get(imageUID);
	}
	xTiles = component.xTiles;
	yTiles = component.yTiles;
	fps = component.fps;
	timer = component.timer;
	frameMix = component.frameMix;
	f1Xpos = component.f1Xpos;
	f1Ypos = component.f1Ypos;
	f2Xpos = component.f2Xpos;
	f2Ypos = component.f2Ypos;

	lifetime = component.lifetime;
	speed = component.speed;
	rate = component.rate;
	rateTimer = 1.f / rate;
	App->particles->AddParticleSystem(this);

}

ComponentParticles::~ComponentParticles()
{
}

Component* ComponentParticles::Clone() const
{
	return new ComponentParticles(*this);
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
			if (texture != nullptr)
			{
				unsigned imageUID = App->resManager->FindByExportedFile(textureName.c_str());
				App->resManager->DeleteResource(imageUID);
				texture = nullptr;
			}
			textureName = None;
		}
		if (none_selected)
			ImGui::SetItemDefaultFocus();

		textureFiles = App->resManager->GetResourceNamesList(TYPE::TEXTURE, true);

		for (int n = 0; n < textureFiles.size(); n++)
		{
			bool is_selected = (textureName == textureFiles[n]);
			if (ImGui::Selectable(textureFiles[n].c_str(), is_selected) && !is_selected)
			{
				App->resManager->DeleteResource(App->resManager->FindByExportedFile(textureName.c_str()));
				textureName = textureFiles[n].c_str();
				unsigned imageUID = App->resManager->FindByExportedFile(textureName.c_str());
				texture = (ResourceTexture*)App->resManager->Get(imageUID);
			}
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	if (texture != nullptr)
	{
		ImGui::Image((ImTextureID)texture->gpuID, { 200,200 }, { 0,1 }, { 1,0 });
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
	ImGui::InputInt("Max Particles", &maxParticles);
	ImGui::DragFloat("Rate", &rate);
	ImGui::InputFloat2("Lifetime", &lifetime[0]);
	ImGui::InputFloat2("Speed", &speed[0]);
	ImGui::InputFloat2("Size", &size[0]);
	ImGui::ColorEdit3("Color", (float*)&particleColor);
	ImGui::PopID();
}

bool ComponentParticles::CleanUp()
{
	unsigned nParticles = particles.size();

	for (; nParticles > 0; --nParticles)
	{
		RELEASE(particles.front());
		particles.pop_front();
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
	math::float3 pos = gameobject->transform->GetGlobalPosition();
	f1Xpos = ((int)frame) % xTiles;
	f2Xpos = (f1Xpos + 1) % xTiles;
	f1Ypos = (((int)frame) / xTiles) % yTiles;
	f2Ypos = f1Xpos < f2Xpos ? f1Ypos : f1Ypos + 1;	

	rateTimer -= dt;
	if (rateTimer <= 0.f && particles.size() < maxParticles)
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
			p->position = pos + float3(rand() % 200, 0, rand() % 200);
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
			p->size = fmod(rand(), abs(size.y - size.x));
			particles.push_back(p);
		}
		rateTimer = 1.f / rate;
	}


	unsigned nParticles = particles.size();
	
	for (; nParticles > 0; --nParticles)
	{
		particles.front()->position += particles.front()->direction * particles.front()->speed * dt;
		float3 direction = (camPos - particles.front()->position);
		particles.front()->global = particles.front()->global.FromTRS(particles.front()->position, math::Quat::LookAt(float3::unitZ, direction.Normalized(), float3::unitY, float3::unitY), math::float3::one * particles.front()->size);
		particles.push_back(particles.front());
		particles.pop_front();
	}
}

void ComponentParticles::Save(JSON_value* value) const
{
	Component::Save(value);
	value->AddInt("xTiles", xTiles);
	value->AddInt("yTiles", yTiles);
	value->AddFloat("fps", fps);
	value->AddString("textureName", textureName.c_str());
	value->AddFloat2("lifetime", lifetime);
	value->AddFloat2("speed", speed);
	value->AddFloat("rate", rate);
	value->AddInt("maxParticles", maxParticles);
	value->AddFloat2("size", size);
	value->AddFloat3("particleColor", particleColor);
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
		texture = (ResourceTexture*)App->resManager->Get(textureName.c_str());
	}
	lifetime = value->GetFloat2("lifetime");
	speed = value->GetFloat2("speed");
	rate = value->GetFloat("rate");
	rateTimer = 1.f / rate;
	maxParticles = value->GetInt("maxParticles");
	size = value->GetFloat2("size");
	particleColor = value->GetFloat3("particleColor");
}


