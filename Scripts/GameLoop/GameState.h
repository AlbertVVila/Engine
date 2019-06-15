#ifndef __GAMESTATE_H_
#define __GAMESTATE_H_

class GameLoop;

class GameState
{
public:
	GameState(GameLoop* GL);
	~GameState();

	virtual void Update() {};
	virtual void Enter() {};
	virtual void Exit() {};


public:

	GameLoop* gLoop = nullptr;

};

#endif // __GAMESTATE_H_