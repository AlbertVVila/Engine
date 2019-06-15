#ifndef __GAMESTATEPLAYING_H_
#define __GAMESTATEPLAYING_H_

#include "GameState.h"

class Button;

class GameStatePlaying :
	public GameState
{
public:
	GameStatePlaying(GameLoop* GL);
	~GameStatePlaying();

	void Update() override;
};

#endif // __GAMESTATEPLAYING_H_