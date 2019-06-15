#ifndef __GAMESTATEPAUSED_H_
#define __GAMESTATEPAUSED_H_

#include "GameState.h"
class GameStatePaused :
	public GameState
{
public:
	GameStatePaused(GameLoop* GL);
	~GameStatePaused();

	void Update() override;
};

#endif // __GAMESTATEPAUSED_H_