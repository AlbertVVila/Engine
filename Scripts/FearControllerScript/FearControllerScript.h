#ifndef  __FearControllerScript_h__
#define  __FearControllerScript_h__

#include "BaseScript.h"
#include <vector>
#include "Math/float3.h"

#ifdef FearControllerScript_EXPORTS
#define FearControllerScript_API __declspec(dllexport)
#else
#define FearControllerScript_API __declspec(dllimport)
#endif

class BasicEnemyAIScript;

class FearControllerScript_API FearControllerScript : public Script
{
	void Awake() override;
	void Start() override;
	void Update() override;

	void Expose(ImGuiContext* context) override;

	void Serialize(JSON_value* json) const override;
	void DeSerialize(JSON_value* json) override;

	inline virtual FearControllerScript* Clone() const
	{
		return new FearControllerScript(*this);
	}

private:
	BasicEnemyAIScript* enemy1 = nullptr;
	BasicEnemyAIScript* enemy2 = nullptr;
	BasicEnemyAIScript* enemy3 = nullptr;

	std::string enemy1Name = "";
	std::string enemy2Name = "";
	std::string enemy3Name = "";

	void LaunchDeathEvent();
	math::float3 LocateDeath();
	bool IsSomeoneDead();
};

#endif __FearControllerScript_h__
