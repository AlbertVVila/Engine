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
	if (finished)
	{
	
	}
}

void BossStateSecondInterPhase::Update()
{
	//Update parts

	switch (state)
	{
		case InterphaseState::None:
			state = InterphaseState::Kneel;
			break;
		case InterphaseState::Kneel:
			state = InterphaseState::Cry;
			break;
		case InterphaseState::Cry:
			state = InterphaseState::FadeOff;
			break;
		case InterphaseState::FadeOff:
			state = InterphaseState::FloorVanish;
			break;
		case InterphaseState::FloorVanish:
			state = InterphaseState::Teleport;
			break;
		case InterphaseState::Teleport:
			state = InterphaseState::FadeIn;
			break;
		case InterphaseState::FadeIn:
			state = InterphaseState::Finished;
			break;
		case InterphaseState::Finished:

			break;
	}
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
