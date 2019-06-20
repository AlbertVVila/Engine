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
	if (player->IsAtacking()) player->currentSkill = (BasicSkill*)player->playerSkills[0];
	if (player->IsUsingFirstSkill()) player->currentSkill = (BasicSkill*)player->playerSkills[1];
	if (player->IsUsingSecondSkill()) player->currentSkill = (BasicSkill*)player->playerSkills[2];
	if (player->IsUsingThirdSkill()) player->currentSkill = (BasicSkill*)player->playerSkills[3];
	if (player->IsUsingFourthSkill()) player->currentSkill = (BasicSkill*)player->playerSkills[4];

	if (player->currentSkill != nullptr)
		player->currentSkill->Start();
}

void PlayerStateAttack::Update()
{
	// Is using skill or basic attack?
	if (player->currentSkill != nullptr)
	{
		player->currentSkill->Update();
	}
}