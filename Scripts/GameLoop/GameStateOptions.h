#ifndef __GAMESTATEOPTIONS_H_
#define __GAMESTATEOPTIONS_H_

#include "GameState.h"

class GameStateOptions :
	public GameState
{
public:
	GameStateOptions(GameLoop* GL);
	~GameStateOptions();

	void Update() override;
};

#endif // __GAMESTATEPAUSED_H_