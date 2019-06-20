#include "PlayerStateAttack.h"
#include "PlayerMovement.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentBoxTrigger.h"

#include "BasicSkill.h"

PlayerStateAttack::PlayerStateAttack(PlayerMovement * PM, const char * trigger, math::float3 boxSize, float minTime, float maxTime):
	PlayerState(PM, trigger, boxSize, minTime, maxTime)
{
}

PlayerStateAttack::~PlayerStateAttack()
{
}

void PlayerStateAttack::Enter()
{
	if(player->IsUsingFirstSkill()) player->currentSkill = (BasicSkill*)player->playerSkills[0];
	if (player->IsUsingSecondSkill()) player->currentSkill = (BasicSkill*)player->playerSkills[1];
	if (player->IsUsingThirdSkill()) player->currentSkill = (BasicSkill*)player->playerSkills[2];
	if (player->IsUsingFourthSkill()) player->currentSkill = (BasicSkill*)player->playerSkills[3];

	if (player->currentSkill != nullptr)
		player->currentSkill->Start();
}

void PlayerStateAttack::Update()
{
	// Is using skill or basic attack?
	if (player->currentSkill != nullptr)
	{
		UseSkill();
	}
	else
	{
		/*if (player->attackBoxTrigger != nullptr && !hitboxCreated && timer > duration * minTime && timer < duration * maxTime)
		{
			//Create the hitbox
			player->attackBoxTrigger->Enable(true);
			player->attackBoxTrigger->SetBoxSize(boxSize);
			boxPosition = player->transform->up * 100.f; //this front stuff isnt working well when rotating the chicken
			player->attackBoxTrigger->SetBoxPosition(boxPosition.x, boxPosition.y, boxPosition.z + 100.f);
			hitboxCreated = true;
		}
		if (player->attackBoxTrigger != nullptr && hitboxCreated && timer > duration * maxTime)
		{
			player->attackBoxTrigger->Enable(false);
			hitboxCreated = false;
		}*/
	}
}

void PlayerStateAttack::UseSkill()
{
	BasicSkill* previous = player->currentSkill;

	player->currentSkill->Update();

	CheckSkills(previous, player->currentSkill);
}

void PlayerStateAttack::CheckSkills(BasicSkill* previous, BasicSkill* current)
{
	if (previous != current)
	{
		if (previous != nullptr)
			previous->Exit();
	}
}