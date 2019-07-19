#ifndef  __FearController_h__
#define  __FearController_h__

#include "BaseScript.h"

#ifdef FearController_EXPORTS
#define FearController_API __declspec(dllexport)
#else
#define FearController_API __declspec(dllimport)
#endif

class BasicEnemyAIScript;

class FearController_API FearController : public Script
{
	void Awake() override;
	void Start() override;
	void Update() override;

	void Expose(ImGuiContext* context) override;

	void Serialize(JSON_value* json) const override;
	void DeSerialize(JSON_value* json) override;

	inline virtual FearController* Clone() const
	{
		return new FearController(*this);
	}

private:
	BasicEnemyAIScript* enemy1 = nullptr;
	BasicEnemyAIScript* enemy2 = nullptr;
	BasicEnemyAIScript* enemy3 = nullptr;

	void LaunchDeathEvent();
	bool IsSomeoneDead();
};

#endif __FearController_h__
