#include "GodMode.h"

#include "Application.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "PlayerMovement/PlayerMovement.h"
#include "GameLoop/GameLoop.h"
#include "imgui.h"

GodMode_API Script* CreateScript()
{
	GodMode* instance = new GodMode;
	return instance;
}

void GodMode::Start()
{
	playerGO = App->scene->FindGameObjectByName("Player");
	gameLoopGO = App->scene->FindGameObjectByName("GameController");
}

void GodMode::Update()
{
	ClickedGodMode();
	if (isGod != wasGod)
	{
		wasGod = isGod;
		if (playerGO != nullptr)
		{
			playerGO->GetComponent<PlayerMovement>()->SetGodMode(isGod);
		}
		else
		{
			LOG("Couldn't find player");
		}
	}
	SwitchLevel();
}

void GodMode::Expose(ImGuiContext* context)
{
	ImGui::Checkbox("GodMode", &isGod);
}

void GodMode::ClickedGodMode()
{
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) != KEY_REPEAT) return;
	if (App->input->GetKey(SDL_SCANCODE_G) != KEY_DOWN) return;
	isGod = !isGod;
}

void GodMode::SwitchLevel()
{
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) != KEY_REPEAT) return;
	unsigned level = 1;
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		level = 1;
	}
	else if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
	{
		level = 2;
	}
	else
	{
		return;
	}
	gameLoopGO->GetComponent<GameLoop>()->LoadLvl(level);
}
