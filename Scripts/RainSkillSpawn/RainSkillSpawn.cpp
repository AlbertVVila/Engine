#include "RainSkillSpawn.h"

#include "Application.h"
#include "ModuleTime.h"

#include "GameObject.h"
#include "ComponentBoxTrigger.h"

#include "imgui.h"
#include "JSON.h"

RainSkillSpawn_API Script* CreateScript()
{
	RainSkillSpawn* instance = new RainSkillSpawn;
	return instance;
}

void RainSkillSpawn::Start()
{
	if (hits > 0)
		hitDuration = duration / hits;
	else
		hitDuration = duration;

	attackBoxTrigger = gameobject->GetComponent<ComponentBoxTrigger>();
	if (attackBoxTrigger == nullptr)
		LOG("Warning: Missing ComponentBoxTrigger in %s.", gameobject->name.c_str());
}

void RainSkillSpawn::Update()
{
	if (start == false)
		Enable(true);

	timer += App->time->fullGameDeltaTime;

	if (timer < duration * hits)
	{
		Rain();
	}
	else
	{
		Reset();
	}
}

void RainSkillSpawn::Rain()
{
	// Check spin state
	if (attackBoxTrigger != nullptr && attackBoxTrigger->enabled)
	{
		// Full spin performed?
		if (hitTimer > hitDuration)
		{
			attackBoxTrigger->Enable(false);
		}
		else
		{
			hitTimer += App->time->gameDeltaTime;
		}
	}
	else
	{
		// Go for next spin
		hitTimer = 0.0f;
		attackBoxTrigger->Enable(true);
	}
}

void RainSkillSpawn::Reset()
{
	attackBoxTrigger->Enable(false);
	gameobject->SetActive(false);
	//RELEASE(gameobject);
}

void RainSkillSpawn::Expose(ImGuiContext* context)
{
	ImGui::InputInt("Hits number", &hits);
	ImGui::InputFloat("Duration", &duration);
}

void RainSkillSpawn::Serialize(JSON_value* json) const
{
	json->AddInt("hits", hits);
	json->AddFloat("duration", duration);
}

void RainSkillSpawn::DeSerialize(JSON_value* json)
{
	hits = json->GetInt("hits");
	duration = json->GetFloat("duration");
}
