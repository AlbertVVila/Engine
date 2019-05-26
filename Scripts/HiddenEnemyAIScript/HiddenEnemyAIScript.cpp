#include "HiddenEnemyAIScript.h"

#include "Application.h"
#include "ModuleTime.h"
#include "ModuleScene.h"

#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentAnimation.h"
#include "ComponentBoxTrigger.h"

#include "EnemyControllerScript.h"
#include "PlayerMovement.h"
#include "EnemyState.h"
#include "EnemyStateWait.h"
#include "EnemyStateShowUp.h"
#include "EnemyStateChase.h"
#include "EnemyStateReturnToStart.h"
#include "EnemyStateHide.h"
#include "EnemyStateAttack.h"
#include "EnemyStateCooldown.h"
#include "EnemyStateDeath.h"

#include "imgui.h"
#include "JSON.h"

HiddenEnemyAIScript_API Script* CreateScript()
{
	HiddenEnemyAIScript* instance = new HiddenEnemyAIScript;
	return instance;
}

void HiddenEnemyAIScript::Start()
{
	enemyStates.reserve(8);

	enemyStates.push_back(wait = new EnemyStateWait(this));
	enemyStates.push_back(showUp = new EnemyStateShowUp(this));
	enemyStates.push_back(chase = new EnemyStateChase(this));
	enemyStates.push_back(returnToStart = new EnemyStateReturnToStart(this));
	enemyStates.push_back(hide = new EnemyStateHide(this));
	enemyStates.push_back(attack = new EnemyStateAttack(this));
	enemyStates.push_back(cooldown = new EnemyStateCooldown(this));
	enemyStates.push_back(death = new EnemyStateDeath(this));
	Appl = App;

	currentState = wait;

	startPosition = GetPosition();
	//startPosition.y += yTranslation;

	// Look for Enemy Controller Script of the enemy
	enemyController = gameobject->GetComponent<EnemyControllerScript>();
	if (enemyController == nullptr)
	{
		LOG("The GameObject %s has no Enemy Controller Script component attached \n", gameobject->name);
	}

	// Look for Player Movement Script
	GameObject* player = App->scene->FindGameObjectByName("Player");
	if (player == nullptr)
	{
		LOG("The player GameObject couldn't be found \n");
	}
	else
	{
		playerScript = player->GetComponent<PlayerMovement>();
		if (playerScript == nullptr)
		{
			LOG("The GameObject %s has no Player Movement Script component attached \n", player->name);
		}
	}

	// Look for Component Animation of the enemy
	anim = (ComponentAnimation*)gameobject->GetComponentInChildren(ComponentType::Animation);
	if (anim == nullptr)
	{
		LOG("No child of the GameObject %s has an Animation component attached \n", gameobject->name);
	}

	boxTrigger = (ComponentBoxTrigger*)gameobject->GetComponentInChildren(ComponentType::BoxTrigger);
	if (boxTrigger == nullptr)
	{
		LOG("No child of the GameObject %s has a boxTrigger component attached \n", gameobject->name);
	}

	LOG("Started hidden enemy AI script");
}

void HiddenEnemyAIScript::Update()
{
	if (enemyController->GetHealth() <= 0)
	{
		currentState = (EnemyState*)death;
		return;
	}

	EnemyState* previous = currentState;

	if (currentState != death)
	{
		currentState->UpdateTimer();

		currentState->Update();

		// If previous and current are different the functions Exit() and Enter() are called
		CheckStates(previous, currentState);
	}
}

void HiddenEnemyAIScript::Expose(ImGuiContext* context)
{
	ImGui::SetCurrentContext(context);
	ImGui::Separator();
	ImGui::Text("Enemy:");

	if (currentState == wait)				ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Wait");
	else if (currentState == showUp)		ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Show Up");
	else if (currentState == chase)			ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Chase");
	else if (currentState == returnToStart)	ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Return");
	else if (currentState == hide)			ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Hide");
	else if (currentState == attack)		ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Attack");
	else if (currentState == cooldown)		ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Cooldown");
	else if (currentState == death)			ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Dead");

	ImGui::Text("Show up:");
	ImGui::InputFloat("Distance to activate", &activationDistance);
	ImGui::InputFloat("Show-up Speed", &showUpSpeed);
	ImGui::Text("Chase:");
	ImGui::InputFloat("Y Translation", &yTranslation);
	ImGui::InputFloat("Chase Speed", &chaseSpeed);
	ImGui::Text("Return:");
	ImGui::InputFloat("Return Distance", &returnDistance);
	ImGui::InputFloat("Return Speed", &returnSpeed);
	ImGui::Text("Attack:");
	ImGui::InputFloat("Attack Time", &attackDuration);
	ImGui::InputFloat("Attack Damage", &attackDamage);
	ImGui::Text("Cooldown:");
	ImGui::InputFloat("Cooldown Time", &cooldownTime);
}

void HiddenEnemyAIScript::Serialize(JSON_value* json) const
{
	assert(json != nullptr);

	//Wait variables
	json->AddFloat("activationDistance", activationDistance);

	// Stand-Up variables
	json->AddFloat("showUpSpeed", showUpSpeed);
	json->AddFloat("yTranslation", yTranslation);

	// Chase variables
	json->AddFloat("chaseSpeed", chaseSpeed);

	// Return variables
	json->AddFloat("returnSpeed", returnSpeed);
	json->AddFloat("returnDistance", returnDistance);

	// Attack variables
	json->AddFloat("attackDuration", attackDuration);
	json->AddFloat("attackDamage", attackDamage);

	// Cooldown variables
	json->AddFloat("cooldownTime", cooldownTime);
}

void HiddenEnemyAIScript::DeSerialize(JSON_value* json)
{
	assert(json != nullptr);

	//Wait variables
	activationDistance = json->GetFloat("activationDistance");

	// Stand-Up variables
	showUpSpeed = json->GetFloat("showUpSpeed");
	yTranslation = json->GetFloat("yTranslation");

	// Chase variables
	chaseSpeed = json->GetFloat("chaseSpeed");

	// Return variables
	returnSpeed = json->GetFloat("returnSpeed");
	returnDistance = json->GetFloat("returnDistance");

	// Attack variables
	attackDuration = json->GetFloat("attackDuration");
	attackDamage = json->GetFloat("attackDamage");

	// Cooldown variables
	cooldownTime = json->GetFloat("cooldownTime");
}

inline math::float3 HiddenEnemyAIScript::GetPosition() const
{
	assert(gameobject->transform != nullptr);
	return gameobject->transform->GetGlobalPosition();
}

inline math::float3 HiddenEnemyAIScript::GetPlayerPosition() const
{
	assert(enemyController != nullptr && enemyController->player != nullptr && enemyController->player->transform != nullptr);
	return enemyController->player->transform->GetGlobalPosition();
}

inline float HiddenEnemyAIScript::GetDistanceTo(math::float3& position) const
{
	math::float3 enemyDistance = GetPosition();
	return enemyDistance.Distance(position);
}

inline float HiddenEnemyAIScript::GetDistanceTo2D(math::float3& position) const
{
	math::float3 enemyDistance = GetPosition();
	enemyDistance.y = position.y;
	return enemyDistance.Distance(position);
}

inline bool HiddenEnemyAIScript::IsCollidingWithPlayer() const
{
	assert(enemyController != nullptr && enemyController->myBbox != nullptr && enemyController->playerBbox != nullptr);
	return enemyController->myBbox->Intersects(*enemyController->playerBbox);
}

void HiddenEnemyAIScript::MoveTowards(float speed) const
{
	math::float3 movement = gameobject->transform->front.Normalized() * -speed * App->time->gameDeltaTime;
	gameobject->transform->SetPosition(gameobject->transform->GetPosition() + movement);
}

void HiddenEnemyAIScript::Move(float speed, math::float3& direction) const
{
	math::float3 movement = direction.Normalized() * speed * App->time->gameDeltaTime;
	gameobject->transform->SetPosition(gameobject->transform->GetPosition() + movement);
}

void HiddenEnemyAIScript::LookAt2D(math::float3& position)
{
	math::float3 auxPos = position;
	auxPos.y = GetPosition().y;
	gameobject->transform->LookAt(auxPos);
}

void HiddenEnemyAIScript::CheckStates(EnemyState* previous, EnemyState* current)
{
	if (previous != current)
	{
		previous->ResetTimer();

		previous->Exit();
		current->Enter();

		if (anim != nullptr)
		{
			anim->SendTriggerToStateMachine(current->trigger.c_str());
		}
	}
}