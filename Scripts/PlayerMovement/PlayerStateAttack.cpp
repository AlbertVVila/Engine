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
	/*SkillType skillType = SkillType::NONE;

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
	else if (player->IsUsingTwo())
	{
		player->currentSkill = player->allSkills[player->assignedSkills[HUB_BUTTON_2]]->skill;
		skillType = player->allSkills[player->assignedSkills[HUB_BUTTON_2]]->type;
	}
	else if (player->IsUsingThree())
	{
		player->currentSkill = player->allSkills[player->assignedSkills[HUB_BUTTON_3]]->skill;
		skillType = player->allSkills[player->assignedSkills[HUB_BUTTON_3]]->type;
	}
	else if (player->IsUsingFour())
	{
		player->currentSkill = player->allSkills[player->assignedSkills[HUB_BUTTON_4]]->skill;
		skillType = player->allSkills[player->assignedSkills[HUB_BUTTON_4]]->type;
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
	}*/
}

void PlayerStateAttack::Update()
{
	// Call current skill
	/*if (player->currentSkill != nullptr)
	{
		player->currentSkill->Update();
	}
	else
	{
		//player->currentState = (PlayerState*)player->idle;
	}*/
}