#include "BasicSkill.h"

#include "Application.h"
#include "ModuleTime.h"

#include "ComponentAnimation.h"
#include "ResourceStateMachine.h"

#include "PlayerState.h"
#include "PlayerMovement.h"
#include "PlayerStateAttack.h"

#include <assert.h>
#include "JSON.h"

BasicSkill::BasicSkill(PlayerMovement* PM, const char* trigger) :
	player(PM), animTrigger(trigger)
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
	player->attack->trigger = animTrigger;
}

void BasicSkill::Update()
{
	timer += player->App->time->gameDeltaTime;

	if (timer < player->currentState->duration)
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
	//ImGui::InputFloat("Duration", &duration);
}

void BasicSkill::Serialize(JSON_value* json) const
{
	assert(json != nullptr);
	//json->AddFloat("Speed", speed);
	//json->AddFloat("Duration", duration);
}

void BasicSkill::DeSerialize(JSON_value* json)
{
	assert(json != nullptr);
	//speed = json->GetFloat("Speed");
	//duration = json->GetFloat("Duration");
}

void BasicSkill::Reset()
{
	timer = 0.0f;
	//Enable(false);

	player->currentSkill->Exit();
	player->currentSkill = nullptr;
}