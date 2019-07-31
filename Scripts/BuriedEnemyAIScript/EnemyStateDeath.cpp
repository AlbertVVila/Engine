#include "EnemyStateDeath.h"



EnemyStateDeath::EnemyStateDeath(BuriedEnemyAIScript* AIScript)
{
	enemy = AIScript;
	trigger = "Death";
}


EnemyStateDeath::~EnemyStateDeath()
{
}
