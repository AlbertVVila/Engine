#ifndef __GAMESTATEMENU_H_
#define __GAMESTATEMENU_H_

#include "GameState.h"

class Button;

class GameStateMenu :
	public GameState
{
public:
	GameStateMenu(GameLoop* GL);
	~GameStateMenu();

	void Update() override;
};

#endif // __GAMESTATEMENU_H_