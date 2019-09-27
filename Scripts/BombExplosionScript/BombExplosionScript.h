#ifndef  __BombExplosionScript_h__
#define  __BombExplosionScript_h__

#include "BaseScript.h"

#ifdef BombExplosionScript_EXPORTS
#define BombExplosionScript_API __declspec(dllexport)
#else
#define BombExplosionScript_API __declspec(dllimport)
#endif

enum class ExplosionState
{
	None,
	Appear,
	Grow,
	Explode
};

class BombExplosionScript_API BombExplosionScript : public Script
{
	void Awake() override;
	void Start() override;
	void Update() override;
	void OnTriggerEnter(GameObject* go) override;
	void Expose(ImGuiContext* context) override;

	void Serialize(JSON_value* json) const override;
	void DeSerialize(JSON_value* json) override;

	inline virtual BombExplosionScript* Clone() const
	{
		return new BombExplosionScript(*this);
	}
private:
	ExplosionState currentState = ExplosionState::None;
};

#endif __BombExplosionScript_h__
