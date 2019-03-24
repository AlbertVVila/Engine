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

class GameLoop_API GameLoop : public Script
{
	enum class GameState
	{		
		MENU,
		INTRO,
		PLAYING,
		PAUSED,
		OPTIONS,
		CREDITS,
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
	void ManageQuit();

	void EnableMenuButtons(bool enable);
	void VolumeManagement();

	void ChangeGameState(GameState newState); //Set initial conditions for each state here if required

	GameState gameState = GameState::MENU;

	//UI Values
	int volume = 10;
	int minVolume = 0;
	int maxVolume = 20;

	//UI

	//Buttons
	std::vector<Component*> menuButtons;
	ComponentButton* optionButton = nullptr;
	ComponentButton* backOptionButton = nullptr;
	std::vector<Component*> volumeButtons;

	//GO
	GameObject* menu = nullptr;
	GameObject* options = nullptr;

	//TEXT
	ComponentText* volumeText = nullptr;

	//Script
	//Script* intro = nullptr;
	Script* playerScript = nullptr;
};

#endif __GameLoop_h__
