#ifndef  __GameLoop_h__
#define  __GameLoop_h__

#include "BaseScript.h"

#ifdef GameLoop_EXPORTS
#define GameLoop_API __declspec(dllexport)
#else
#define GameLoop_API __declspec(dllimport)
#endif

#include <vector>

class Component;
class ComponentButton;
class ComponentText;
class PlayerMovement;

class GameLoop_API GameLoop : public Script
{
	enum class GameState
	{		
		MENU,
		INTRO,
		PLAYING,
		DEAD,
		PAUSED,
		OPTIONS,
		CREDITS,
		CONTROLS,
		QUIT
	};

	void Start() override;
	void Update() override;

	void ManageMenu();
	void ManageIntro();
	void ManagePlaying();
	void ManagePaused();
	void ManageOptions();
	void ManageCredits();
	void ManageControls();
	void ManageQuit();

	void EnableMenuButtons(bool enable);
	void VolumeManagement();
	void SoundManagement();

	void ChangeGameState(GameState newState); //Set initial conditions for each state here if required

	GameState gameState = GameState::MENU;

	//UI Values
	int volume = 10;
	int minVolume = 0;
	int maxVolume = 10;
	int sound = 6;
	int minSound = 0;
	int maxSound = 10;

	//UI

	//Buttons
	std::vector<Component*> menuButtons;
	ComponentButton* optionButton = nullptr;
	ComponentButton* controlsButton = nullptr;
	ComponentButton* backOptionButton = nullptr;
	ComponentButton* backControlsButton = nullptr;
	ComponentButton* toTheAltarButton = nullptr;
	std::vector<Component*> volumeButtons;
	std::vector<Component*> soundButtons;

	//GO
	GameObject* menu = nullptr;
	GameObject* options = nullptr;
	GameObject* controls = nullptr;
	GameObject* loseWindow = nullptr;
	GameObject* winWindow = nullptr;
	GameObject* playerGO = nullptr;

	//TEXT
	ComponentText* volumeText = nullptr;
	ComponentText* soundText = nullptr;

	//Script
	//Script* intro = nullptr;
	PlayerMovement* playerScript = nullptr;
};

#endif __GameLoop_h__
