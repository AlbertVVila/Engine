#ifndef  __FlyingSkullEnemyAIScript_h__
#define  __FlyingSkullEnemyAIScript_h__

#ifdef FlyingSkullEnemyAIScript_EXPORTS
#define FlyingSkullEnemyAIScript_API __declspec(dllexport)
#else
#define FlyingSkullEnemyAIScript_API __declspec(dllimport)
#endif

#include "BaseScript.h"
#include "math/float3.h"
#include <vector>

class GameObject;
class EnemyControllerScript;
class JSON_value;

class FlyingSkullEnemyAIScript_API FlyingSkullEnemyAIScript : public Script
{
	void Awake() override;
	void Start() override;
	void Update() override;

	void Expose(ImGuiContext* context) override;

	void Serialize(JSON_value* json) const override;
	void DeSerialize(JSON_value* json) override;

	inline virtual FlyingSkullEnemyAIScript* Clone() const
	{
		return new FlyingSkullEnemyAIScript(*this);
	}

};

#endif __FlyingSkullEnemyAIScript_h__
