#ifndef __GAMESTATECONTROLS_H_
#define __GAMESTATECONTROLS_H_

#include "GameState.h"

class GameLoop;
class Button;

class GameStateControls :
	public GameState
{
public:
	GameStateControls(GameLoop* GL);
	~GameStateControls();

	void Update() override;
};

#endif // __GAMESTATECONTROLS_H_