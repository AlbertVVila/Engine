#include "BaseSkill.h"

#include "Application.h"
#include "ModuleTime.h"

#include "PlayerMovement.h"

#include <assert.h>
#include "JSON.h"

BaseSkill::BaseSkill(PlayerMovement* PM) :
	player(PM)
{
}

BaseSkill::~BaseSkill()
{
}

void BaseSkill::Start()
{
	if (player == nullptr)
	{
		LOG("Warning: Player GO pointer missing");
	}
}

void BaseSkill::Update()
{
	timer += player->App->time->gameDeltaTime;

	if (timer > duration)
	{
		UseSkill();
	}
	else
	{
		Reset();
	}
}

void BaseSkill::Expose(ImGuiContext* context)
{
	ImGui::SetCurrentContext(context);
	//ImGui::InputFloat("Speed", &speed);
	ImGui::InputFloat("Duration", &duration);
}

void BaseSkill::Serialize(JSON_value* json) const
{
	assert(json != nullptr);
	//json->AddFloat("Speed", speed);
	json->AddFloat("Duration", duration);
}

void BaseSkill::DeSerialize(JSON_value* json)
{
	assert(json != nullptr);
	//speed = json->GetFloat("Speed");
	duration = json->GetFloat("Duration");
}

void BaseSkill::Reset()
{
	timer = 0.0f;
	//Enable(false);
}