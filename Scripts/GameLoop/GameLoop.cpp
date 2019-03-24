#include "GameLoop.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "ComponentButton.h"
#include "ComponentText.h"
#include "Globals.h"

GameLoop_API Script* CreateScript()
{
	GameLoop* instance = new GameLoop;
	return instance;
}

void GameLoop::Start()
{
	LOG("Started GameLoop script");
	menu = App->scene->FindGameObjectByName("Menu");
	assert(menu != nullptr);

	options = App->scene->FindGameObjectByName("OptionsMenu");
	assert(options != nullptr);

	GameObject* mainMenuGO = App->scene->FindGameObjectByName("MainMenu");
	assert(mainMenuGO != nullptr);
	menuButtons = mainMenuGO->GetComponentsInChildren(ComponentType::Button);

	GameObject* optionsGO = App->scene->FindGameObjectByName("ButtonOptions");
	assert(optionsGO != nullptr);
	optionButton = (ComponentButton*)optionsGO->GetComponent(ComponentType::Button);

	GameObject* musicGO = App->scene->FindGameObjectByName("MusicRow");
	assert(musicGO != nullptr);
	volumeButtons = musicGO->GetComponentsInChildren(ComponentType::Button);

	GameObject* volumeGO = App->scene->FindGameObjectByName(musicGO, "MusicNumberText");
	volumeText = (ComponentText*) volumeGO->GetComponent(ComponentType::Text);//MusicNumberText

	GameObject* backOptionsGO = App->scene->FindGameObjectByName(options, "Back");
	backOptionButton = (ComponentButton *)backOptionsGO->GetComponent(ComponentType::Button);
	assert(backOptionButton != nullptr);

	GameObject* playerGO = App->scene->FindGameObjectByName("Player");
	playerScript = (Script*)playerGO->GetScript();
	assert(playerScript != nullptr);
}

void GameLoop::Update()
{
	//Test loop
	/*
	if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_DOWN)
	{
		gameState = GameState::MENU;
	}
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN)
	{
		gameState = GameState::PLAYING;
	}
	if (App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN)
	{
		gameState = GameState::PAUSED;
	}
	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
	{
		gameState = GameState::CREDITS;
	}
	if (App->input->GetKey(SDL_SCANCODE_T) == KEY_DOWN)
	{
		gameState = GameState::QUIT;
	}
	if (App->input->GetKey(SDL_SCANCODE_Y) == KEY_DOWN)
	{
		gameState = GameState::INTRO;
	}
	*/
	//Test loop - End
	switch (gameState)
	{
		case GameState::MENU:
		{
			//Manage menu stuff
			ManageMenu();
			break;
		}
		case GameState::INTRO:
		{
			//When the intro is done go to PLAYING
			ManageIntro();
			break;
		}
		case GameState::PLAYING:
		{
			//Update player / enemies / check for collisions
			ManagePlaying();
			break;
		}
		case GameState::PAUSED:
		{
			//Wait until pause is removed // No update player / enemies / no check for collisions
			ManagePaused();
			break;
		}
		case GameState::OPTIONS:
		{
			//Show credits
			ManageOptions();
			break;
		}
		case GameState::CREDITS:
		{
			//Show credits
			ManageCredits();
			break;
		}
		case GameState::QUIT:
		{
			//Close App
			ManageQuit();
			break;
		}

	}
}

void GameLoop::ManageMenu()
{
	if (((ComponentButton*)menuButtons[0])->IsPressed()) //PlayButton
	{
		menu->SetActive(false);
		gameState = GameState::INTRO;
	}
	else if (optionButton->IsPressed())
	{
		options->SetActive(true);
		EnableMenuButtons(false);
		gameState = GameState::OPTIONS;
	}
}

void GameLoop::ManageIntro()
{
}

void GameLoop::ManagePlaying()
{

}

void GameLoop::ManagePaused()
{
}

void GameLoop::ManageOptions()
{
	VolumeManagement();
	if (backOptionButton->IsPressed())
	{
		options->SetActive(false);
		EnableMenuButtons(true);
		gameState = GameState::MENU;
	}
}

void GameLoop::ManageCredits()
{
}

void GameLoop::ManageQuit()
{
}

void GameLoop::EnableMenuButtons(bool enable)
{
	for (int i = 0; i < menuButtons.size(); i++)
	{
		menuButtons[i]->Enable(enable);
	}
}

void GameLoop::VolumeManagement()
{
	if (((ComponentButton*)volumeButtons[0])->IsPressed()) //Decrease
	{
		volume = MAX(minVolume, volume - 1);
		volumeText->text = std::to_string(volume);
	}
	else if (((ComponentButton*)volumeButtons[1])->IsPressed()) //Increase
	{
		volume = MIN(maxVolume, volume + 1);
		volumeText->text = std::to_string(volume);
	}
}

void GameLoop::ChangeGameState(GameState newState) //Set initial conditions for each state here if required
{
	switch (newState) 
	{
		case GameState::MENU:
		{		
			break;
		}
		case GameState::INTRO:
		{	
			break;
		}
		case GameState::PLAYING:
		{	
			break;
		}
		case GameState::PAUSED:
		{	
			break;
		}
		case GameState::CREDITS:
		{	
			break;
		}
		case GameState::QUIT:
		{	
			break;
		}
	}

	gameState = newState;
}
