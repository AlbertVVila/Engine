#ifndef __GAMESTATEDEAD_H_
#define __GAMESTATEDEAD_H_

#include "GameState.h"
class GameStateDead :
	public GameState
{
public:
	GameStateDead(GameLoop* GL);
	~GameStateDead();

	void Update() override;
};

#endif // __GAMESTATEDEAD_H_