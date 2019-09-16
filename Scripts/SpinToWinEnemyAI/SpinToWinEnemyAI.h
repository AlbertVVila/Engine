#ifndef  __SpinToWinEnemyAI_h__
#define  __SpinToWinEnemyAI_h__

#include "BasicEnemyAIScript/BasicEnemyAIScript.h"

#ifdef SpinToWinEnemyAI_EXPORTS
#define SpinToWinEnemyAI_API __declspec(dllexport)
#else
#define SpinToWinEnemyAI_API __declspec(dllimport)
#endif

class SpinToWinEnemyAI_API SpinToWinEnemyAI : public BasicEnemyAIScript
{
public:
	inline virtual SpinToWinEnemyAI* Clone() const
	{
		return new SpinToWinEnemyAI(*this);
	}
	void Start() override;
	void Update() override;

public:
};

extern "C" SpinToWinEnemyAI_API Script* CreateScript();
#endif __DividingEnemyAI_h__
