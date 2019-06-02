#include "EnemyLifeBarController.h"

#include "Application.h"
#include "ModuleScene.h"

#include "GameObject.h"
#include "ComponentTransform2D.h"

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

	life = hPbar->GetComponent<Transform2D>();
	lifeBarMaxWidht = life->getSize();
	lifeBarPositionX = life->getPosition();
}

void EnemyLifeBarController::Update()
{
	lifeBackground->SetActive(false);
	hPbar->SetActive(false);
	enemyTypeName->SetActive(false);
	skull->SetActive(false);
	boneRight->SetActive(false);
	boneLeft->SetActive(false);
	life->SetSize(lifeBarMaxWidht);
	life->SetPositionUsingAligment(lifeBarPositionX);
}

void EnemyLifeBarController::SetLifeBar(int maxHP, int actualHP, EnemyLifeBarType type, std::string name)
{
	life->SetSize(lifeBarMaxWidht);
	life->SetPositionUsingAligment(lifeBarPositionX);
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

	float actualWidth = (actualHP * life->getSize().x) / maxHP;

	float difference = life->getSize().x - actualWidth;

	life->SetSize(math::float2(life->getSize().x - difference, life->getSize().y));
	math::float2 newPosition = math::float2(life->getPosition().x - (difference / 2), life->getPosition().y);
	life->SetPositionUsingAligment(newPosition);
}
