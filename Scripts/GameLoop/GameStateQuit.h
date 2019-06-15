#ifndef __GAMESTATEQUIT_H_
#define __GAMESTATEQUIT_H_

#include "GameState.h"
class GameStateQuit :
	public GameState
{
public:
	GameStateQuit(GameLoop* GL);
	~GameStateQuit();

	void Update() override;
};

#endif // __GAMESTATEQUIT_H_