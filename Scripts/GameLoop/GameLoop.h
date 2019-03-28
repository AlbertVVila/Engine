#ifndef  __GameLoop_h__
#define  __GameLoop_h__

#include "BaseScript.h"

#include "Math/float3.h"

#ifdef GameLoop_EXPORTS
#define GameLoop_API __declspec(dllexport)
#else
#define GameLoop_API __declspec(dllimport)
#endif

#include <vector>

class Component;
class ComponentButton;
class ComponentText;
class ComponentScript;
class PlayerMovement;
class EnemyMovementScript;
class IntroScript;
class AABB;

class GameLoop_API GameLoop : public Script
{
	enum class GameState
	{		
		MENU,
		INTRO,
		PLAYING,
		DEAD,
		WIN,
		PAUSED,
		OPTIONS,
		CREDITS,
		CONTROLS,
		QUIT
	};

	void Start() override;
	void Update() override;

	void ManageDead();

	void ManageMenu();
	void ManageIntro();
	void ManagePlaying();
	void ManageWin();
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

	int loading = 0;

	//UI

	//Buttons
	std::vector<Component*> menuButtons;
	ComponentButton* optionButton = nullptr;
	ComponentButton* controlsButton = nullptr;
	ComponentButton* exitButton = nullptr;
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
	GameObject* hudGO = nullptr;
	GameObject* playerGO = nullptr;
	GameObject* enemyGO = nullptr;
	GameObject* creditsGO = nullptr;

	//BBOX
	math::AABB* playerBbox = nullptr;
	math::AABB* winBbox = nullptr;

	//TEXT
	ComponentText* volumeText = nullptr;
	ComponentText* soundText = nullptr;

	ComponentScript* componentIntroScript = nullptr;

	//Script
	PlayerMovement* playerScript = nullptr;
	EnemyMovementScript* enemyMovementScript = nullptr;
	IntroScript* introScript = nullptr;

	float3 playerStartPosition = float3::zero;
	float3 enemyStartPosition = float3::zero;

	bool runningCredits = false;
};

#endif __GameLoop_h__
