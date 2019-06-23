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
	/*if (player->IsAtacking()) player->currentSkill = (BasicSkill*)player->playerSkills[0];
	if (player->IsUsingFirstSkill()) player->currentSkill = (BasicSkill*)player->playerSkills[1];
	if (player->IsUsingSecondSkill()) player->currentSkill = (BasicSkill*)player->playerSkills[2];
	if (player->IsUsingThirdSkill()) player->currentSkill = (BasicSkill*)player->playerSkills[3];
	if (player->IsUsingFourthSkill()) player->currentSkill = (BasicSkill*)player->playerSkills[4];*/
	SkillType skillType = SkillType::NONE;

	if (player->IsAtacking())
	{
		player->currentSkill = player->allSkills[player->assignedSkills[0]]->skill;
		skillType = player->allSkills[player->assignedSkills[0]]->type;
	}
	else if (player->IsUsingQ())
	{
		player->currentSkill = player->allSkills[player->assignedSkills[1]]->skill;
		skillType = player->allSkills[player->assignedSkills[1]]->type;
	}
	else if (player->IsUsingW())
	{
		player->currentSkill = player->allSkills[player->assignedSkills[2]]->skill;
		skillType = player->allSkills[player->assignedSkills[2]]->type;
	}
	else if (player->IsUsingE())
	{
		player->currentSkill = player->allSkills[player->assignedSkills[3]]->skill;
		skillType = player->allSkills[player->assignedSkills[3]]->type;
	}
	else if (player->IsUsingR())
	{
		player->currentSkill = player->allSkills[player->assignedSkills[4]]->skill;
		skillType = player->allSkills[player->assignedSkills[4]]->type;
	}
	else if (player->IsUsingOne())
	{
		player->currentSkill = player->allSkills[player->assignedSkills[5]]->skill;
		skillType = player->allSkills[player->assignedSkills[5]]->type;
	}
	
	if (player->currentSkill != nullptr)
	{
		player->UseSkill(skillType);
		player->currentSkill->Start();
	}
}

void PlayerStateAttack::Update()
{
	// Call current skill
	if (player->currentSkill != nullptr)
	{
		player->currentSkill->Update();
	}
}