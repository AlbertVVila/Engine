#include "BasicSkill.h"

#include "Application.h"
#include "ModuleTime.h"

#include "PlayerMovement.h"

#include <assert.h>
#include "JSON.h"

BasicSkill::BasicSkill(PlayerMovement* PM) :
	player(PM)
{
}

BasicSkill::~BasicSkill()
{
}

void BasicSkill::Start()
{
	if (player == nullptr)
	{
		LOG("Warning: Player GO pointer missing");
	}
}

void BasicSkill::Update()
{
	timer += player->App->time->gameDeltaTime;

	if (timer < duration)
	{
		UseSkill();
	}
	else
	{
		Reset();
	}
}

void BasicSkill::Exit()
{
	player->currentState = (PlayerState*)player->idle;
}

void BasicSkill::Expose(ImGuiContext* context)
{
	ImGui::SetCurrentContext(context);
	//ImGui::InputFloat("Speed", &speed);
	ImGui::InputFloat("Duration", &duration);
}

void BasicSkill::Serialize(JSON_value* json) const
{
	assert(json != nullptr);
	//json->AddFloat("Speed", speed);
	json->AddFloat("Duration", duration);
}

void BasicSkill::DeSerialize(JSON_value* json)
{
	assert(json != nullptr);
	//speed = json->GetFloat("Speed");
	duration = json->GetFloat("Duration");
}

void BasicSkill::Reset()
{
	timer = 0.0f;
	//Enable(false);

	player->currentSkill = nullptr;
}