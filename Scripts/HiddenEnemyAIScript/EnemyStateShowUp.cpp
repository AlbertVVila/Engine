#include "EnemyStateShowUp.h"

#include "GameObject.h"
#include "ComponentTransform.h"

#include "HiddenEnemyAIScript.h"

EnemyStateShowUp::EnemyStateShowUp(HiddenEnemyAIScript* AIScript)
{
	enemy = AIScript;
	trigger = "ShowUp";
}

EnemyStateShowUp::~EnemyStateShowUp()
{
}

void EnemyStateShowUp::Update()
{
	// Translate on the Y axis
	enemy->Move(enemy->showUpSpeed, enemy->gameobject->transform->up);
	//auxTranslation += movement.y;

	// Check if the needed Y has been reached
	if (enemy->startPosition.y + enemy->yTranslation <= enemy->GetPosition().y)
	{
		math::float3 position = enemy->startPosition;
		position.y += enemy->yTranslation;
		enemy->gameobject->transform->SetPosition(position);
		enemy->currentState = (EnemyState*)enemy->chase;
		auxTranslation = 0.0f;
	}
}
