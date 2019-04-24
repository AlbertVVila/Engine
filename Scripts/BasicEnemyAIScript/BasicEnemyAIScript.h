#ifndef  __BasicEnemyAIScript_h__
#define  __BasicEnemyAIScript_h__

#include "BaseScript.h"

#ifdef BasicEnemyAIScript_EXPORTS
#define BasicEnemyAIScript_API __declspec(dllexport)
#else
#define BasicEnemyAIScript_API __declspec(dllimport)
#endif

class BasicEnemyAIScript_API BasicEnemyAIScript : public Script
{
	void Start() override;

};

#endif __BasicEnemyAIScript_h__
