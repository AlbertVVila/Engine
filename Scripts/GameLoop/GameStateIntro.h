#ifndef __GAMESTATEINTRO_
#define __GAMESTATEINTRO_

#include "GameState.h"

class GameStateIntro : public GameState
{
public:
	GameStateIntro(GameLoop* GL);
	~GameStateIntro();

	void Update() override;
};

#endif // __GAMESTATEINTRO_