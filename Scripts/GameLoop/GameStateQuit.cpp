#include "GameStateQuit.h"



GameStateQuit::GameStateQuit(GameLoop* GL) : GameState(GL)
{
}


GameStateQuit::~GameStateQuit()
{
}

void GameStateQuit::Update()
{
	App->scripting->status = UPDATE_STOP;
}
