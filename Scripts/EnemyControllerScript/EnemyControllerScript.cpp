#include "EnemyControllerScript.h"

#include "Application.h"
#include "ModuleScene.h"

#include "GameObject.h"
#include "ComponentRenderer.h"
#include "ComponentTransform.h"

#include "DamageController.h"
#include "EnemyLifeBarController.h"

#include "imgui.h"
#include "JSON.h"

EnemyControllerScript_API Script* CreateScript()
{
	EnemyControllerScript* instance = new EnemyControllerScript;
	return instance;
}

void EnemyControllerScript::Start()
{
	player = App->scene->FindGameObjectByName(App->scene->root, playerName.c_str());
	GameObject* myRenderGO = App->scene->FindGameObjectByName(gameobject, myBboxName.c_str());
	if(myRenderGO != nullptr)
		myRender = (ComponentRenderer*)myRenderGO->GetComponentOld(ComponentType::Renderer);

	myBbox = &App->scene->FindGameObjectByName(gameobject, myBboxName.c_str())->bbox;
	playerBbox = &App->scene->FindGameObjectByName(player, playerBboxName.c_str())->bbox;
	damageController = App->scene->FindGameObjectByName("Damage")->GetComponent<DamageController>();
	enemyLifeBar = App->scene->FindGameObjectByName("EnemyLife")->GetComponent<EnemyLifeBarController>();
}

void EnemyControllerScript::Update()
{
	math::float3 closestPoint;
	if (App->scene->Intersects(closestPoint, "BasicEnemyMesh"))
	{
		enemyLifeBar->SetLifeBar(maxHealth, actualHealth, EnemyLifeBarType::NORMAL, "Skeleton");
		myRender->highlighted = true;
	}
	else
	{
		myRender->highlighted = false;
	}
}

void EnemyControllerScript::Expose(ImGuiContext * context)
{
	char* bboxName = new char[64];
	strcpy_s(bboxName, strlen(myBboxName.c_str()) + 1, myBboxName.c_str());
	ImGui::InputText("My BBox Name", bboxName, 64);
	myBboxName = bboxName;
	delete[] bboxName;

	if (ImGui::InputInt("Health", &maxHealth))
	{
		actualHealth = maxHealth;
	}

	ImGui::Separator();
	ImGui::Text("Player:");
	char* goName = new char[64];
	strcpy_s(goName, strlen(playerName.c_str()) + 1, playerName.c_str());
	ImGui::InputText("playerName", goName, 64);
	playerName = goName;
	delete[] goName;

	char* targetBboxName = new char[64];
	strcpy_s(targetBboxName, strlen(playerBboxName.c_str()) + 1, playerBboxName.c_str());
	ImGui::InputText("Player BBox Name", targetBboxName, 64);
	playerBboxName = targetBboxName;
	delete[] targetBboxName;
}

void EnemyControllerScript::Serialize(JSON_value* json) const
{
	assert(json != nullptr);
	json->AddString("playerName", playerName.c_str());
	json->AddString("playerBboxName", playerBboxName.c_str());
	json->AddString("myBboxName", myBboxName.c_str());
	json->AddInt("health", maxHealth);
}

void EnemyControllerScript::DeSerialize(JSON_value* json)
{
	assert(json != nullptr);
	playerName = json->GetString("playerName");
	playerBboxName = json->GetString("playerBboxName");
	myBboxName = json->GetString("myBboxName");
	maxHealth = json->GetInt("health", maxHealth);
	actualHealth = maxHealth;
}

void EnemyControllerScript::TakeDamage(unsigned damage)
{
	if (gameobject->isActive())
	{
		if (actualHealth - damage < 0)
		{
			actualHealth = 0;
			gameobject->SetActive(false);
		}
		else
		{
			actualHealth -= damage;
		}

		damageController->AddDamage(gameobject->transform, damage, 2);
	}
}
