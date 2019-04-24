#include "BasicEnemyAIScript.h"

#include "Application.h"
#include "ModuleTime.h"
#include "ModuleScene.h"

#include "ComponentTransform.h"
#include "GameObject.h"

#include "Math/MathFunc.h"
#include "imgui.h"
#include <stack>

BasicEnemyAIScript_API Script* CreateScript()
{
	BasicEnemyAIScript* instance = new BasicEnemyAIScript;
	return instance;
}

void BasicEnemyAIScript::Start()
{
	player = GetGameObjectRecursiveByName(App->scene->root, playerName.c_str());

	myBbox = &App->scene->FindGameObjectByName(gameObject, "ZombMesh")->bbox;
}

void BasicEnemyAIScript::Update()
{
	switch (enemyState)
	{
	case EnemyState::STANDUP:
		StandUp();
		break;
	case EnemyState::CHASE:
		LOG("Chasing");
		break;
	case EnemyState::ATTACK:
		break;
	case EnemyState::COOLDOWN:
		break;
	case EnemyState::DEAD:
		break;
	default:
		break;
	}
}

void BasicEnemyAIScript::Expose(ImGuiContext* context)
{
	ImGui::SetCurrentContext(context);
	ImGui::InputFloat("Stand-up Speed", &standupSpeed);
	ImGui::InputFloat("Y Translation", &yTranslation);

	char* goName = new char[64];
	strcpy_s(goName, strlen(playerName.c_str()) + 1, playerName.c_str());
	ImGui::InputText("playerName", goName, 64);
	playerName = goName;
	delete[] goName;
}

void BasicEnemyAIScript::StandUp()
{
	// Start playing stand-up animation

	// Translate on the Z axis
	float3 movement = gameObject->transform->up.Normalized() * standupSpeed * App->time->gameDeltaTime;
	gameObject->transform->SetPosition(gameObject->transform->GetPosition() + movement);
	auxTranslation += movement.y;

	// Check if the needed Z has been reached
	if (yTranslation <= auxTranslation)
	{
		enemyState = EnemyState::CHASE;
		auxTranslation = 0.0f;
	}
}

void BasicEnemyAIScript::Chase()
{
	float3 enemyCurrentPosition = gameObject->transform->GetGlobalPosition();
	float3 playerCurrentPosition = player->transform->GetGlobalPosition();

	float3 dir = (enemyCurrentPosition - playerCurrentPosition);
	Quat currentRotation = gameObject->transform->GetRotation();
	Quat rotation = currentRotation.LookAt(gameObject->transform->front.Normalized(), dir.Normalized(), float3::unitY, float3::unitY);

	gameObject->transform->SetRotation(rotation.Mul(currentRotation));

	float3 movement = gameObject->transform->front.Normalized() * -chaseSpeed * App->time->gameDeltaTime;
	gameObject->transform->SetPosition(gameObject->transform->GetPosition() + movement);

	if (myBbox->Intersects(*player->GetBoundingBox()))
	{
		playerScript->isPlayerDead = true;
		stop = true;
	}
}

GameObject* BasicEnemyAIScript::GetGameObjectRecursiveByName(GameObject* gameObject, const char* name)
{
	if (strcmp(gameObject->name.c_str(), name) == 0)
	{
		return gameObject;
	}

	std::stack<GameObject*> stack;
	for (GameObject* childre : gameObject->children)
	{
		stack.push(childre);
	}

	while (!stack.empty())
	{
		GameObject* gameObject = stack.top();
		stack.pop();

		if (strcmp(gameObject->name.c_str(), name) == 0)
		{
			return gameObject;
		}

		for (GameObject* childre : gameObject->children)
		{
			stack.push(childre);
		}
	}
}