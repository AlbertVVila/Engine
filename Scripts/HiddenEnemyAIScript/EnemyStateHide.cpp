#include "EnemyStateHide.h"

#include "GameObject.h"
#include "ComponentTransform.h"

#include "HiddenEnemyAIScript.h"

EnemyStateHide::EnemyStateHide(HiddenEnemyAIScript* AIScript)
{
	enemy = AIScript;
	trigger = "Hide";
}


EnemyStateHide::~EnemyStateHide()
{
}

void EnemyStateHide::Update()
{
	// Translate on the Y axis
	math::float3 direction = enemy->gameobject->transform->up;
	direction.y *= -1.0f;
	enemy->Move(enemy->showUpSpeed, direction);
	//auxTranslation += movement.y;

	// Check if the needed Y has been reached
	if (enemy->startPosition.y >= enemy->GetPosition().y)
	{
		enemy->gameobject->transform->SetPosition(enemy->startPosition);
		enemy->currentState = (EnemyState*)enemy->wait;
		auxTranslation = 0.0f;
	}
}
