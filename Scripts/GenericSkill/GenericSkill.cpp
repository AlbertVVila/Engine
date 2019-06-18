#include "GenericSkill.h"

#include "Application.h"
#include "ModuleTime.h"

#include "imgui.h"
#include "JSON.h"

GenericSkill::GenericSkill(PlayerMovement* PM) :
	player(PM)
{
}


GenericSkill::~GenericSkill()
{
}

void GenericSkill::Start()
{
	if (player == nullptr)
	{
		LOG("Warning: Player GO pointer missing");
	}
}

void GenericSkill::Update()
{
	timer += App->time->gameDeltaTime;

	if (timer > duration)
	{
		UseSkill();
	}
	else
	{
		Reset();
	}
}

void GenericSkill::Expose(ImGuiContext* context)
{
	ImGui::SetCurrentContext(context);
	//ImGui::InputFloat("Speed", &speed);
	ImGui::InputFloat("Duration", &duration);
}

void GenericSkill::Serialize(JSON_value* json) const
{
	assert(json != nullptr);
	//json->AddFloat("Speed", speed);
	json->AddFloat("Duration", duration);
}

void GenericSkill::DeSerialize(JSON_value* json)
{
	assert(json != nullptr);
	//speed = json->GetFloat("Speed");
	duration = json->GetFloat("Duration");
}

void GenericSkill::Reset()
{
	timer = 0.0f;
	//Enable(false);
}
