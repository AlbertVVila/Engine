#ifndef __GAMESTATECREDITS_H_
#define __GAMESTATECREDITS_H_

#include "GameState.h"

class CreditsScript;

class GameStateCredits :
	public GameState
{
public:
	GameStateCredits(GameLoop* GL);
	~GameStateCredits();

	void Enter() override;
	void Update() override;
};

#endif // __GAMESTATECREDITS_H_