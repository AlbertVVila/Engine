#include "BossStateCutScene.h"

#include "BossBehaviourScript.h"
#include "EnemyControllerScript/EnemyControllerScript.h"

BossStateCutScene::BossStateCutScene(BossBehaviourScript* AIBoss)
{
}

BossStateCutScene::~BossStateCutScene()
{
}

void BossStateCutScene::HandleIA()
{
	//if finished everything change to summon army
}

void BossStateCutScene::Update()
{
	//do the lerping in

	//Close door

	//lerping to boss

	//back to player

}

void BossStateCutScene::Enter()
{
	//Deactivate player script
}

void BossStateCutScene::Exit()
{
	//Activate player script
}
