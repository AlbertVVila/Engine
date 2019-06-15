#ifndef __GAMESTATEWIN_H_
#define __GAMESTATEWIN_H_

#include "GameState.h"

class GameStateWin : public GameState
{
public:
	GameStateWin(GameLoop* GL);
	~GameStateWin();

	void Update() override;
};

#endif // __GAMESTATEWIN_H_