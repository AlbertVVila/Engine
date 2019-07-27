#ifndef __ENEMYSTATEMULTIPLY_H_
#define __ENEMYSTATEMULTIPLY_H_

#include "BasicEnemyAIScript\EnemyState.h"
class BasicEnemyAIScript;

class EnemyStateMultiply :
	public EnemyState
{
public:
	EnemyStateMultiply(BasicEnemyAIScript* AIScript);
	~EnemyStateMultiply();

	void HandleIA() override;
private:
	bool hasMultiplied = false;
};

#endif __ENEMYSTATEMULTIPLY_H_
