#ifndef  __GameLoop_h__
#define  __GameLoop_h__

#include "BaseScript.h"

#ifdef GameLoop_EXPORTS
#define GameLoop_API __declspec(dllexport)
#else
#define GameLoop_API __declspec(dllimport)
#endif

class ComponentButton;

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

	void ChangeGameState(GameState newState); //Set initial conditions for each state here if required

	GameState gameState = GameState::MENU;

	//Buttons

	ComponentButton* playButton = nullptr;
	ComponentButton* optionButton = nullptr;
	ComponentButton* backOptionButton = nullptr;


	//GO
	GameObject* menu = nullptr;
	GameObject* options = nullptr;

	//Script
	//Script* intro = nullptr;
};

extern "C" GameLoop_API GameLoop* CreateScript();

#endif __GameLoop_h__
