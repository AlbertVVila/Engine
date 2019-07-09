#include "GodMode.h"

#include "Application.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "PlayerMovement/PlayerMovement.h"
#include "ExperienceController/ExperienceController.h"
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
	xpGO = App->scene->FindGameObjectByName("Xp");
}

void GodMode::Update()
{
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) != KEY_REPEAT) return;
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
	AddExperience();
	SwitchLevel();
}

void GodMode::Expose(ImGuiContext* context)
{
	ImGui::Checkbox("GodMode", &isGod);
}

void GodMode::ClickedGodMode()
{
	if (App->input->GetKey(SDL_SCANCODE_G) != KEY_DOWN) return;
	isGod = !isGod;
}

void GodMode::SwitchLevel() const
{
	unsigned level = 1u;
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		level = 1u;
	}
	else if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
	{
		level = 2u;
	}
	else
	{
		return;
	}
	gameLoopGO->GetComponent<GameLoop>()->LoadLvl(level);
}

void GodMode::AddExperience() const
{
	if (App->input->GetKey(SDL_SCANCODE_E) != KEY_DOWN) return;
	xpGO->GetComponent<ExperienceController>()->AddXP(100.f);
}
