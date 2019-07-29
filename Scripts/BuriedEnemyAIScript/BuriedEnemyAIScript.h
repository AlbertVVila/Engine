#ifndef  __BuriedEnemyAIScript_h_
#define  __BuriedEnemyAIScript_h_

#ifdef BuriedEnemyAIScript_EXPORTS
#define BuriedEnemyAIScript_API __declspec(dllexport)
#else
#define BuriedEnemyAIScript_API __declspec(dllimport)
#endif

#include "BaseScript.h"

#include <vector>

class EnemyControllerScript;

class BuriedEnemyAIScript_API BuriedEnemyAIScript : public Script
{

	EnemyControllerScript* enemyController = nullptr;
};

extern "C" BuriedEnemyAIScript_API Script* CreateScript();

#endif __BuriedEnemyAIScript_h_
