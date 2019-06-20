#include "GameLoop.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "ModuleScript.h"
#include "ModuleRender.h"
#include "ModuleTime.h"

#include "GameObject.h"
#include "ComponentButton.h"
#include "ComponentText.h"
#include "ComponentTransform.h"
#include "ComponentCamera.h"
#include "ComponentTransform2D.h"
#include "ComponentImage.h"

#include "PlayerMovement.h"
#include "IntroScript.h"
#include "CreditsScript.h"

#include "Globals.h"

#include "JSON.h"
#include "imgui.h"

#include "ComponentAudioSource.h"

#define MENU_SCENE "MenuScene"
#define GRAVEYARD_SCENE "Level0-TheGraveyard"

GameLoop_API Script* CreateScript()
{
	GameLoop* instance = new GameLoop;
	return instance;
}

void GameLoop::Expose(ImGuiContext* context)
{
	ImGui::SetCurrentContext(context);
	const char * scenes[] = { "Menu","Graveyard"};
	if (ImGui::BeginCombo("Scene", scenes[(int)gameScene]))
	{
		for (int n = 0; n < 2; n++)
		{
			bool isSelected = ((int)gameScene == n);
			if (ImGui::Selectable(scenes[n], isSelected) && (int)gameScene != n)
			{
				gameScene = (GameScene)n;
			}
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
}

void GameLoop::Start()
{
	if (gameScene == GameScene::MENU)
	{
		LoadMenuScene();
		gameState = GameState::MENU;
	}
	else if (gameScene == GameScene::CEMENTERY)
	{
		LoadHUDScene();
		LoadCementeryScene();
		App->scene->root->OnPlay();
		gameState = GameState::INTRO;
	}

	if (App->scene->actionAfterLoad != -1)
	{
		ChangeGameState((GameState)App->scene->actionAfterLoad);
		App->scene->actionAfterLoad = -1;
	}
}

void GameLoop::Update()
{
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
	case GameState::WIN:
	{
		ManageWin();
		break;
	}
	case GameState::DEAD:
	{
		ManageDead();
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
	case GameState::CONTROLS:
	{
		//Show credits
		ManageControls();
		break;
	}
	case GameState::QUIT:
	{
		//Close App
		ManageQuit();
		break;
	}
	case GameState::LOADING:
	{
		ManageLoading();
		break;
	}
	}
}


void GameLoop::LoadMenuScene()
{
	LOG("Started GameLoop script");
	menu = App->scene->FindGameObjectByName("Menu");
	assert(menu != nullptr);

	options = App->scene->FindGameObjectByName("OptionsMenu");
	assert(options != nullptr);

	controls = App->scene->FindGameObjectByName("ControlsMenu");
	assert(controls != nullptr);

	GameObject* mainMenuGO = App->scene->FindGameObjectByName("MainMenu");
	assert(mainMenuGO != nullptr);
	menuButtons = mainMenuGO->GetComponentsInChildren(ComponentType::Button);

	GameObject* optionsGO = App->scene->FindGameObjectByName("ButtonOptions");
	assert(optionsGO != nullptr);
	optionButton = optionsGO->GetComponent<Button>();

	GameObject* controlsGO = App->scene->FindGameObjectByName("ButtonControls");
	assert(controlsGO != nullptr);
	controlsButton = controlsGO->GetComponent<Button>();

	GameObject* creditsButtonGO = App->scene->FindGameObjectByName("ButtonCredits");
	assert(creditsButtonGO != nullptr);
	creditsButton = creditsButtonGO->GetComponent<Button>();

	GameObject* musicGO = App->scene->FindGameObjectByName("MusicRow");
	assert(musicGO != nullptr);
	volumeButtons = musicGO->GetComponentsInChildren(ComponentType::Button);

	GameObject* volumeGO = App->scene->FindGameObjectByName("MusicNumberText", musicGO);
	volumeText = (Text*)volumeGO->GetComponent<Text>();//MusicNumberText

	GameObject* soundGO = App->scene->FindGameObjectByName("SoundRow");
	assert(soundGO != nullptr);
	soundButtons = soundGO->GetComponentsInChildren(ComponentType::Button);

	GameObject* soundVolumeGO = App->scene->FindGameObjectByName("SoundNumberText", soundGO);
	soundText = (Text*)soundVolumeGO->GetComponent<Text>();//SoundNumberText

	vsyncGO = App->scene->FindGameObjectByName("Check", options);
	assert(vsyncGO != nullptr);

	GameObject* vsyncButtonGO = App->scene->FindGameObjectByName("CheckBackground", options);
	vsyncButton = (Button*)vsyncButtonGO->GetComponent<Button>();
	assert(vsyncButton != nullptr);

	GameObject* backOptionsGO = App->scene->FindGameObjectByName("Back", options);
	backOptionButton = (Button *)backOptionsGO->GetComponent<Button>();
	assert(backOptionButton != nullptr);

	GameObject* backControlsGO = App->scene->FindGameObjectByName("Back", controls);
	backControlsButton = (Button *)backControlsGO->GetComponent<Button>();
	assert(backControlsButton != nullptr);

	GameObject* exitButtonGO = App->scene->FindGameObjectByName("ButtonExit");
	exitButton = (Button*)exitButtonGO->GetComponent<Button>();
	assert(exitButton != nullptr);

	creditsGO = App->scene->FindGameObjectByName("Credits");
	assert(creditsGO != nullptr);

	GameObject* backCreditsGO = App->scene->FindGameObjectByName("Back", creditsGO);
	backCreditsButton = (Button *)backCreditsGO->GetComponent<Button>();
	assert(backCreditsButton != nullptr);

	GameObject* imageCredits = App->scene->FindGameObjectByName("ImageCredits", creditsGO);
	creditsScript = imageCredits->GetComponent<CreditsScript>();
	assert(creditsScript != nullptr);

	loadingGO = App->scene->FindGameObjectByName("Loading");
	assert(loadingGO != nullptr);


}

void GameLoop::LoadHUDScene()
{
	loseWindow = App->scene->FindGameObjectByName("LoseWindow");
	assert(loseWindow != nullptr);

	GameObject* toTheAltarGO = App->scene->FindGameObjectByName("Button", loseWindow);
	toTheAltarButton = (Button *)toTheAltarGO->GetComponent<Button>();
	assert(toTheAltarButton != nullptr);

	winWindow = App->scene->FindGameObjectByName("WinWindow");
	assert(winWindow != nullptr);

	hudGO = App->scene->FindGameObjectByName("GameHUB");
	assert(winWindow != nullptr);

	GameObject* backMenuGO = App->scene->FindGameObjectByName("MenuButton", hudGO);
	hudBackToMenuButton = (Button *)backMenuGO->GetComponent<Button>();
	assert(hudBackToMenuButton != nullptr);

	GameObject* inventoryGO = App->scene->FindGameObjectByName("InventaryButton", hudGO);
	inventoryButton = (Button *)inventoryGO->GetComponent<Button>();
	assert(inventoryButton != nullptr);

	GameObject* missionsGO = App->scene->FindGameObjectByName("MissionButton", hudGO);
	missionsButton = (Button *)missionsGO->GetComponent<Button>();
	assert(missionsButton != nullptr);

	GameObject* skillsGO = App->scene->FindGameObjectByName("SkillsButton", hudGO);
	skillsButton = (Button *)skillsGO->GetComponent<Button>();
	assert(skillsButton != nullptr);

	playerMenuGO = App->scene->FindGameObjectByName("PlayerMenu");
	assert(playerMenuGO != nullptr);

	GameObject* closeButton = App->scene->FindGameObjectByName("CloseButton", playerMenuGO);
	closePlayerMenuButton = (Button *)closeButton->GetComponent<Button>();
	assert(closePlayerMenuButton != nullptr);

	inventoryMenuGO = App->scene->FindGameObjectByName("Inventory", playerMenuGO);
	assert(inventoryMenuGO != nullptr);

	missionsMenuGO = App->scene->FindGameObjectByName("Quests", playerMenuGO);
	assert(missionsMenuGO != nullptr);

	skillsMenuGO = App->scene->FindGameObjectByName("Skills", playerMenuGO);
	assert(skillsMenuGO != nullptr);
}

void GameLoop::LoadCementeryScene()
{
	playerGO = App->scene->FindGameObjectByName("Player");
	playerScript = playerGO->GetComponent<PlayerMovement>();
	assert(playerScript != nullptr);

	playerBbox = &App->scene->FindGameObjectByName("PlayerMesh", playerGO)->bbox;

	//enemyGO = App->scene->FindGameObjectByName("Enemy");
	//enemyMovementScript = (EnemyMovementScript*)enemyGO->GetScript();
	//assert(enemyMovementScript != nullptr);

	winBbox = &App->scene->FindGameObjectByName("WinZone")->bbox;
	assert(winBbox != nullptr);

	introCamera = App->scene->FindGameObjectByName("IntroCamera");
	assert(introCamera != nullptr);
	componentIntroCamera = (ComponentCamera*)introCamera->GetComponent<ComponentCamera>();
	introScript = introCamera->GetComponent<IntroScript>();
	assert(introScript != nullptr);

	loadingGO = App->scene->FindGameObjectByName("Loading");
	assert(loadingGO != nullptr);

	ChangeGameState(GameState::INTRO);
	//componentIntroCamera->SetAsMain();
	hudGO->SetActive(false);

	menuButtonsSound = App->scene->FindGameObjectByName("MenuButtonsAudio")->GetComponent<ComponentAudioSource>();
	assert(menuButtons != nullptr);
}


void GameLoop::ManageDead()
{
	if (toTheAltarButton->IsPressed())
	{
		loseWindow->SetActive(false);
		loadingGO->SetActive(true);
		playerMenuGO->SetActive(false);
		ChangeGameState(GameState::LOADING);
		sceneToLoad = GRAVEYARD_SCENE;
		actionAfterLoad = (int)GameState::INTRO;
	}
}

void GameLoop::ManageMenu()
{
	if (((Button*)menuButtons[0])->IsPressed()) //PlayButton
	{
		ChangeGameState(GameState::LOADING);
		menu->SetActive(false);
		loadingGO->SetActive(true);
		sceneToLoad = GRAVEYARD_SCENE;
	}
	else if (optionButton->IsPressed())
	{
		options->SetActive(true);
		EnableMenuButtons(false);
		ChangeGameState(GameState::OPTIONS);
	}
	else if (controlsButton->IsPressed())
	{
		controls->SetActive(true);
		EnableMenuButtons(false);
		ChangeGameState(GameState::CONTROLS);
	}
	else if (creditsButton->IsPressed())
	{
		
		ChangeGameState(GameState::CREDITS);
	}
	else if (exitButton->IsPressed())
	{
		ChangeGameState(GameState::QUIT);
	}
}

void GameLoop::ManageIntro()
{

	//if (!runningIntro)
	//{
	//	introCamera->SetActive(true);
	//	introScript->Start(); //TODO should start script first time it's enabled
	//	runningIntro = true;
	//}

	/*if (introScript->introDone)
	{*/
		hudGO->SetActive(true);
		GameObject* playerCameraGO = App->scene->FindGameObjectByName("PlayerCamera");
		ComponentCamera* camera = (ComponentCamera*)playerCameraGO->GetComponent<ComponentCamera>();
		camera->SetAsMain();
		/*camera->isMainCamera = true;
		if (App->scene->maincamera != nullptr)
		{
			App->scene->maincamera->isMainCamera = false;
		}
		App->scene->maincamera = camera;*/
		/*introCamera->SetActive(false);
		runningIntro = false;
		introScript->introDone = false;
		componentIntroCamera->gameobject->transform->SetPosition(introScript->initialPosition);*/
		ChangeGameState(GameState::PLAYING);
	//}

}

void GameLoop::ManagePlaying()
{

	if (hudBackToMenuButton->IsPressed())
	{
		ChangeGameState(GameState::LOADING);
		playerMenuGO->SetActive(false);
		hudGO->SetActive(false);
		loadingGO->SetActive(true);
		sceneToLoad = MENU_SCENE;
	}

	if (closePlayerMenuButton->KeyUp())
	{
		closePlayerMenuButton->isHovered = false;
		playerMenuGO->SetActive(false);
		inventoryButton->rectTransform->setPosition(math::float2(-50, inventoryButton->rectTransform->getPosition().y));
		skillsButton->rectTransform->setPosition(math::float2(-50, skillsButton->rectTransform->getPosition().y));
		missionsButton->rectTransform->setPosition(math::float2(-50, missionsButton->rectTransform->getPosition().y));

	}
	if (inventoryButton->IsPressed() && !inventoryMenuGO->isActive()) menuButtonsSound->Play();
	if (skillsButton->IsPressed() && !skillsMenuGO->isActive()) menuButtonsSound->Play();
	if (missionsButton->IsPressed() && !missionsMenuGO->isActive()) menuButtonsSound->Play();

	if (inventoryButton->IsPressed() || skillsButton->IsPressed() || missionsButton->IsPressed())
	{
		//menuButtonsSound->Play();
		playerMenuGO->SetActive(true);
		inventoryButton->rectTransform->setPosition(math::float2(-485, inventoryButton->rectTransform->getPosition().y));
		skillsButton->rectTransform->setPosition(math::float2(-485, skillsButton->rectTransform->getPosition().y));
		missionsButton->rectTransform->setPosition(math::float2(-485, missionsButton->rectTransform->getPosition().y));
		inventoryMenuGO->SetActive(inventoryButton->IsPressed());
		skillsMenuGO->SetActive(skillsButton->IsPressed());
		missionsMenuGO->SetActive(missionsButton->IsPressed());
	}

	playerScript->canInteract = true;
	if (inventoryButton->IsHovered() || skillsButton->IsHovered() || missionsButton->IsHovered() || closePlayerMenuButton->IsHovered())
	{
		playerScript->canInteract = false;
	}
	else if (playerMenuGO->activeSelf && HasImageHoveredInChildren(playerMenuGO))
	{
		playerScript->canInteract = false;
	}

	if (playerScript->isPlayerDead)
	{
		loseWindow->SetActive(true);
		ChangeGameState(GameState::DEAD);
	}
	else if (winBbox->Intersects(*playerBbox))
	{
		winWindow->SetActive(true);
		ChangeGameState(GameState::WIN);
	}
}

void GameLoop::ManageWin()
{
	loading += App->time->gameDeltaTime;

	if (loading >= 1.0f)
	{
		loading = 0.0;
		//ResetPositions();
		ChangeGameState(GameState::LOADING);
		playerMenuGO->SetActive(false);
		sceneToLoad = MENU_SCENE;
		actionAfterLoad = (int)GameState::CREDITS;
	}
}

void GameLoop::ManagePaused()
{
}

void GameLoop::ManageOptions()
{
	VolumeManagement();
	SoundManagement();
	VsyncManagement();
	ResolutionManagement();
	if (backOptionButton->IsPressed())
	{
		backOptionButton->isHovered = false;
		backOptionButton->isKeyDown = false;
		options->SetActive(false);
		EnableMenuButtons(true);
		ChangeGameState(GameState::MENU);
	}
}

void GameLoop::ManageCredits()
{
	if (!runningCredits)
	{
		creditsGO->SetActive(true);
		creditsScript->Enable(true);
		creditsScript->Start();
		runningCredits = true;
	}
	else if (creditsScript->creditsDone)
	{
		creditsGO->SetActive(false);
		menu->SetActive(true);
		runningCredits = false;
		creditsScript->ResetScript();
		ChangeGameState(GameState::MENU);
	}

	if (backCreditsButton->IsPressed())
	{
		creditsScript->creditsDone = true;
	}
}

void GameLoop::ManageControls()
{
	if (backControlsButton->IsPressed())
	{
		backControlsButton->isHovered = false;
		backControlsButton->isKeyDown = false;
		controls->SetActive(false);
		EnableMenuButtons(true);
		ChangeGameState(GameState::MENU);
	}
}

void GameLoop::ManageQuit()
{
	App->scripting->status = UPDATE_STOP;
}

void GameLoop::ManageLoading()
{
	App->scene->loadScene = true;
	App->scene->name = sceneToLoad;
	App->scene->actionAfterLoad = actionAfterLoad;
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
	if (((Button*)volumeButtons[0])->IsPressed()) //Decrease
	{
		volume = MAX(minVolume, volume - 1);
		volumeText->text = std::to_string(volume);
	}
	else if (((Button*)volumeButtons[1])->IsPressed()) //Increase
	{
		volume = MIN(maxVolume, volume + 1);
		volumeText->text = std::to_string(volume);
	}
}

void GameLoop::SoundManagement()
{
	if (((Button*)soundButtons[0])->IsPressed()) //Decrease
	{
		sound = MAX(minSound, sound - 1);
		soundText->text = std::to_string(sound);
	}
	else if (((Button*)soundButtons[1])->IsPressed()) //Increase
	{
		sound = MIN(maxSound, sound + 1);
		soundText->text = std::to_string(sound);
	}
}

void GameLoop::VsyncManagement()
{
	if (vsyncButton->IsPressed())
	{
		vsync = !vsync;
		vsyncGO->SetActive(vsync);
		App->renderer->SetVsync(vsync);
	}
}

void GameLoop::ResolutionManagement()
{
	// 1280/720 - 1600/900 - 1920/1080
}

void GameLoop::ResetPositions()
{
	//playerGO->transform->SetPosition(playerStartPosition);
	//enemyGO->transform->SetPosition(enemyStartPosition);
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
	case GameState::DEAD:
	{
		break;
	}
	case GameState::WIN:
	{
		break;
	}
	case GameState::PAUSED:
	{
		break;
	}
	case GameState::CREDITS:
	{
		menu->SetActive(false);
		break;
	}
	case GameState::QUIT:
	{
		break;
	}
	}

	gameState = newState;
}

bool GameLoop::HasImageHoveredInChildren(const GameObject * go) const
{
	for (auto image : go->GetComponentsInChildren(ComponentType::Image))
	{
		if (((ComponentImage*)image)->isHovered)
		{
			return true;
		}
	}
	return false;
}

void GameLoop::Serialize(JSON_value* json) const
{
	assert(json != nullptr);
	json->AddInt("scene", (int)gameScene);
}

void GameLoop::DeSerialize(JSON_value* json)
{
	assert(json != nullptr);
	gameScene = (GameScene)json->GetInt("scene");
}