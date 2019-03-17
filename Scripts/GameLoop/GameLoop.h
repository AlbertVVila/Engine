#ifndef  __GameLoop_h__
#define  __GameLoop_h__

#include "BaseScript.h"

#ifdef GameLoop_EXPORTS
#define GameLoop_API __declspec(dllexport)
#else
#define GameLoop_API __declspec(dllimport)
#endif

class GameLoop_API GameLoop : public Script
{
	enum class GameState
	{		
		MENU,
		INTRO,
		PLAYING,
		PAUSED,
		CREDITS,
		QUIT
	};

	void Start() override;
	void Update() override;

	void ManageMenu();
	void ManageIntro();
	void ManagePlaying();
	void ManagePaused();
	void ManageCredits();
	void ManageQuit();

	void ChangeGameState(GameState newState); //Set initial conditions for each state here if required

	GameState gameState = GameState::MENU;

	//Buttons

	GameObject* playButton = nullptr;
};

extern "C" GameLoop_API GameLoop* CreateScript();

#endif __GameLoop_h__
