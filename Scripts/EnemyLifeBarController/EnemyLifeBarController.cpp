#include "EnemyLifeBarController.h"

#include "Application.h"
#include "ModuleScene.h"

#include "GameObject.h"
#include "ComponentImage.h"

EnemyLifeBarController_API Script* CreateScript()
{
	EnemyLifeBarController* instance = new EnemyLifeBarController;
	return instance;
}

void EnemyLifeBarController::Start()
{
	GameObject* enemyLife = App->scene->FindGameObjectByName("EnemyLife");
	lifeBackground = App->scene->FindGameObjectByName("LifeBackground", enemyLife);
	hPbar = App->scene->FindGameObjectByName("HPbar", enemyLife);
	enemyTypeName = App->scene->FindGameObjectByName("EnemyTypeName", enemyLife);
	boneRight = App->scene->FindGameObjectByName("BoneRight", enemyLife);
	boneLeft = App->scene->FindGameObjectByName("BoneLeft", enemyLife);
	skull = App->scene->FindGameObjectByName("Skull", enemyLife);

	lifeImage = hPbar->GetComponent<ComponentImage>();
}

void EnemyLifeBarController::Update()
{
	lifeBackground->SetActive(false);
	hPbar->SetActive(false);
	enemyTypeName->SetActive(false);
	skull->SetActive(false);
	boneRight->SetActive(false);
	boneLeft->SetActive(false);
	//if this is the enemy that was being targeted, we untarget it from the scene
	if (App->scene->enemyHovered != nullptr &&
		gameobject->UUID == App->scene->enemyHovered->UUID)
	{
		App->scene->enemyHovered = nullptr;
	}
}

void EnemyLifeBarController::SetLifeBar(int maxHP, int actualHP, EnemyLifeBarType type, std::string name)
{
	//we set the scene target to this enemy
	App->scene->enemyHovered = gameobject;

	lifeBackground->SetActive(true);
	hPbar->SetActive(true);
	enemyTypeName->SetActive(true);
	switch (type)
	{
	case EnemyLifeBarType::NORMAL:
		skull->SetActive(false);
		boneRight->SetActive(false);
		boneLeft->SetActive(false);
		break;
	case EnemyLifeBarType::HARD:
		skull->SetActive(true);
		boneRight->SetActive(false);
		boneLeft->SetActive(false);
		break;
	case EnemyLifeBarType::BOSS:
		skull->SetActive(true);
		boneRight->SetActive(true);
		boneLeft->SetActive(true);
		break;
	default:
		break;
	}

	lifeImage->SetMaskAmount((actualHP * 100) / maxHP);
}

