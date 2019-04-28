#ifndef  __EnemyControllerScript_h__
#define  __EnemyControllerScript_h__

#include "BaseScript.h"

#ifdef EnemyControllerScript_EXPORTS
#define EnemyControllerScript_API __declspec(dllexport)
#else
#define EnemyControllerScript_API __declspec(dllimport)
#endif

class EnemyControllerScript_API EnemyControllerScript : public Script
{
	void Start() override;
};

#endif __EnemyControllerScript_h__
