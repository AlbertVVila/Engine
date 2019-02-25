#include "PlayerMovement.h"

#include "Application.h"
#include "ModuleInput.h"

#include "ComponentTransform.h"
#include "GameObject.h"

#include "JSON.h"
#include <assert.h>
#include "imgui.h"
#include <iostream>
#include "Globals.h"

PlayerMovement_API PlayerMovement* CreateScript()
{
	PlayerMovement* instance = new PlayerMovement;
	return instance;
}

void PlayerMovement::Expose(ImGuiContext* context)
{
	ImGui::SetCurrentContext(context);
	ImGui::InputFloat("speed", &speed);
}

void PlayerMovement::Start()
{
	a = gameObject->transform->GetPosition();
	LOG("Started player movement script");
}
void PlayerMovement::Update()
{
	if(App->input->IsKeyPressed(SDL_SCANCODE_W))
	{
		a.z -= speed;
	}
	if (App->input->IsKeyPressed(SDL_SCANCODE_S))
	{
		a.z += speed;
	}
	if (App->input->IsKeyPressed(SDL_SCANCODE_A))
	{
		a.x -= speed;
	}
	if (App->input->IsKeyPressed(SDL_SCANCODE_D))
	{
		a.x += speed;
	}
	gameObject->transform->SetPosition(a);
}

void PlayerMovement::Serialize(JSON_value * json) const
{
	assert(json != nullptr);
	json->AddFloat("speed", speed);
}

void PlayerMovement::DeSerialize(JSON_value * json)
{
	assert(json != nullptr);
	speed = json->GetFloat("speed");
}
