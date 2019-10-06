#include "BossStateSecondInterPhase.h"



BossStateSecondInterPhase::BossStateSecondInterPhase(BossBehaviourScript* AIboss)
{
	boss = AIboss;
}


BossStateSecondInterPhase::~BossStateSecondInterPhase()
{
}

void BossStateSecondInterPhase::HandleIA()
{
	//this will have a fixed duration
}

void BossStateSecondInterPhase::Update()
{
	//Update parts

	//Make her kneel

	//Disappear

	//Throne floor and everything on top needs to disappear. no need to touch the navmesh

	//Teleport her to the designated spot and change her scale

	//change the animation and make her appear again

	//maybe wait a bit idk
}

void BossStateSecondInterPhase::Enter()
{
	//We need to reset variables and get everything ready

	//get the duration to be a sum of all the durations
}

void BossStateSecondInterPhase::Exit()
{

}
