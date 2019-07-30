#include "BuriedEnemyAIScript.h"

#include "EnemyControllerScript.h"
#include "EnemyStateChase.h"
#include "EnemyStateAttack.h"
#include "EnemyStateCooldown.h"
#include "EnemyStateDeath.h"
#include "EnemyStateHidden.h"
#include "EnemyStateHide.h"
#include "EnemyStateShowUp.h"
#include "EnemyStateRelocate.h"
#include "EnemyStateReturnToStart.h"

#include "GameObject.h"
#include "ComponentAnimation.h"

#include "JSON.h"

BuriedEnemyAIScript_API Script* CreateScript()
{
	BuriedEnemyAIScript* instance = new BuriedEnemyAIScript;
	return instance;
}

void BuriedEnemyAIScript::Awake()
{
	// Look for Enemy Controller Script of the enemy
	enemyController = gameobject->GetComponent<EnemyControllerScript>();
	if (enemyController == nullptr)
	{
		LOG("The GameObject %s has no Enemy Controller Script component attached \n", gameobject->name);
	}
}

void BuriedEnemyAIScript::Start()
{
	//Create states
	enemyStates.reserve(9);
	enemyStates.push_back(attack = new EnemyStateAttack(this));
	enemyStates.push_back(cooldown = new EnemyStateCooldown(this));
	enemyStates.push_back(hide = new EnemyStateHide(this));
	enemyStates.push_back(hidden = new EnemyStateHidden(this));
	enemyStates.push_back(showUp = new EnemyStateShowUp(this));
	enemyStates.push_back(returnToStart = new EnemyStateReturnToStart(this));
	enemyStates.push_back(chase = new EnemyStateChase(this));
	enemyStates.push_back(death = new EnemyStateDeath(this));
	enemyStates.push_back(relocate = new EnemyStateRelocate(this));

	startPosition = enemyController->GetPosition();

	currentState = hidden;
}

void BuriedEnemyAIScript::Update()
{
	EnemyState* previous = currentState;

	if (enemyController->GetHealth() <= 0 && currentState != death)
	{
		currentState = (EnemyState*)death;
	}

	currentState->HandleIA();
	currentState->Update();
	
	CheckStates(previous);
}

void BuriedEnemyAIScript::Expose(ImGuiContext * context)
{

	if (currentState == hidden)				ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: hidden");
	else if (currentState == chase)			ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Chase");
	else if (currentState == returnToStart)	ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Return");
	else if (currentState == attack)		ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Attack");
	else if (currentState == cooldown)		ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Cooldown");
	else if (currentState == death)			ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Dead");
	else if (currentState == relocate)		ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: relocating");
	else if (currentState == hide)			ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: hide");
	else if (currentState == showUp)		ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: ShowUp");


	ImGui::InputFloat("attack cooldown duration: ", &cooldownDuration);
	ImGui::InputFloat("activation distance: ", &activationDistance);
	ImGui::InputFloat("disengage distance: ", &disengageDistance);
	ImGui::InputFloat("max attack range: ", &maxAttackRange);
	ImGui::InputFloat("min attack range: ", &minAttackRange);
	ImGui::InputFloat("teleport cooldown: ", &teleportCooldown);
	ImGui::InputFloat("run speed: ", &runSpeed);

}

void BuriedEnemyAIScript::Serialize(JSON_value * json) const
{
	json->AddFloat("cooldownDuration", cooldownDuration);
	json->AddFloat("activationDistance", activationDistance);
	json->AddFloat("disengageDistance", disengageDistance);
	json->AddFloat("maxAttackRange", maxAttackRange);
	json->AddFloat("minAttackRange", minAttackRange);
	json->AddFloat("teleportCooldown", teleportCooldown);
	json->AddFloat("runSpeed", runSpeed);
}

void BuriedEnemyAIScript::DeSerialize(JSON_value * json)
{
	cooldownDuration = json->GetFloat("cooldownDuration");
	activationDistance = json->GetFloat("activationDistance");
	disengageDistance = json->GetFloat("disengageDistance");
	maxAttackRange = json->GetFloat("maxAttackRange");
	minAttackRange = json->GetFloat("minAttackRange");
	teleportCooldown = json->GetFloat("teleportCooldown");
	runSpeed = json->GetFloat("runSpeed");
}

void BuriedEnemyAIScript::CheckStates(EnemyState* previous)
{
	if (previous != currentState)
	{
		previous->ResetTimer();

		previous->Exit();
		currentState->Enter();

		if (enemyController->anim != nullptr)
		{
			enemyController->anim->SendTriggerToStateMachine(currentState->trigger.c_str());
		}

		currentState->duration = enemyController->anim->GetDurationFromClip();
	}
}
