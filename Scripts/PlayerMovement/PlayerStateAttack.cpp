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
		player->currentSkill = player->allSkills[player->assignedSkills[HUB_BUTTON_RC]]->skill;
		skillType = player->allSkills[player->assignedSkills[HUB_BUTTON_RC]]->type;
	}
	else if (player->IsUsingOne())
	{
		player->currentSkill = player->allSkills[player->assignedSkills[HUB_BUTTON_1]]->skill;
		skillType = player->allSkills[player->assignedSkills[HUB_BUTTON_1]]->type;
	}
	else if (player->IsUsingQ())
	{
		player->currentSkill = player->allSkills[player->assignedSkills[HUB_BUTTON_Q]]->skill;
		skillType = player->allSkills[player->assignedSkills[HUB_BUTTON_Q]]->type;
	}
	else if (player->IsUsingW())
	{
		player->currentSkill = player->allSkills[player->assignedSkills[HUB_BUTTON_W]]->skill;
		skillType = player->allSkills[player->assignedSkills[HUB_BUTTON_W]]->type;
	}
	else if (player->IsUsingE())
	{
		player->currentSkill = player->allSkills[player->assignedSkills[HUB_BUTTON_E]]->skill;
		skillType = player->allSkills[player->assignedSkills[HUB_BUTTON_E]]->type;
	}
	else if (player->IsUsingR())
	{
		player->currentSkill = player->allSkills[player->assignedSkills[HUB_BUTTON_R]]->skill;
		skillType = player->allSkills[player->assignedSkills[HUB_BUTTON_R]]->type;
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