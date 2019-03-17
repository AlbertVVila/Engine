#include "EnemyMovementScript.h"


#include "Application.h"
#include "ModuleInput.h"
#include "ModuleScene.h"

#include "ComponentTransform.h"
#include "GameObject.h"

#include "JSON.h"
#include <assert.h>
#include "imgui.h"
#include <iostream>
#include <stack>
#include "Globals.h"

EnemyMovementScript_API EnemyMovementScript* CreateScript()
{
	EnemyMovementScript* instance = new EnemyMovementScript;
	return instance;
}

void EnemyMovementScript::Expose(ImGuiContext* context)
{
	ImGui::SetCurrentContext(context);
	ImGui::InputFloat("speed", &speed);

	char* goName = new char[64];
	strcpy_s(goName, strlen(playerName.c_str()) + 1, playerName.c_str());
	ImGui::InputText("playerName", goName, 64);
	playerName = goName;
	delete[] goName;
}

void EnemyMovementScript::Start()
{
	LOG("Started player movement script");
	player = GetGameObjectRecursiveByName(App->scene->root, playerName.c_str());
}

void EnemyMovementScript::Update()
{
	if (player != nullptr)
	{
		float3 enemyCurrentPosition = gameObject->transform->GetPosition();
		float3 playerCurrentPosition = player->transform->GetPosition();
		
		if (playerCurrentPosition.z > enemyCurrentPosition.z)
		{
			enemyCurrentPosition.z += speed;
		}
		else
		{
			enemyCurrentPosition.z -= speed;
		}

		if (playerCurrentPosition.x > enemyCurrentPosition.x)
		{
			enemyCurrentPosition.x += speed;
		}
		else
		{
			enemyCurrentPosition.x -= speed;
		}

		gameObject->transform->SetPosition(enemyCurrentPosition);
	}
}

void EnemyMovementScript::Serialize(JSON_value* json) const
{
	assert(json != nullptr);
	json->AddFloat("speed", speed);
	json->AddString("playerName", playerName.c_str());
}

void EnemyMovementScript::DeSerialize(JSON_value* json)
{
	assert(json != nullptr);
	speed = json->GetFloat("speed");
	playerName = json->GetString("playerName");
}

GameObject* EnemyMovementScript::GetGameObjectRecursiveByName(GameObject* gameObject, const char* name)
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
