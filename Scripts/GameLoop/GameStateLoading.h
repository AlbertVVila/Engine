#ifndef __GAMESTATELOADING_H_
#define __GAMESTATELOADING_H_

#include "GameState.h"
class GameStateLoading :
	public GameState
{
public:
	GameStateLoading(GameLoop* GL);
	~GameStateLoading();

	void Update() override;
};

#endif // __GAMESTATELOADING_H_