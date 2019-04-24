#include "BasicEnemyAIScript.h"

#include "Application.h"
#include "ModuleTime.h"

#include "ComponentTransform.h"
#include "GameObject.h"

#include "Math/MathFunc.h"
#include "imgui.h"

BasicEnemyAIScript_API Script* CreateScript()
{
	BasicEnemyAIScript* instance = new BasicEnemyAIScript;
	return instance;
}

void BasicEnemyAIScript::Start()
{
}

void BasicEnemyAIScript::Update()
{
	float3 movement;
	switch (enemyState)
	{
	case EnemyState::STANDUP:
		// Start playing stand-up animation

		// Translate on the Z axis
		movement = gameObject->transform->up.Normalized() * standupSpeed * App->time->gameDeltaTime;
		gameObject->transform->SetPosition(gameObject->transform->GetPosition() + movement);
		auxTranslation += movement.y;

		// Check if the needed Z has been reached
		if (yTranslation <= auxTranslation)
		{
			enemyState = EnemyState::CHASE;
			auxTranslation = 0.0f;
		}
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
}
