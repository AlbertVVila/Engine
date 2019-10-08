#ifndef  __ThirdPhaseAOE_h__
#define  __ThirdPhaseAOE_h__

#include "BaseScript.h"

#ifdef ThirdPhaseAOE_EXPORTS
#define ThirdPhaseAOE_API __declspec(dllexport)
#else
#define ThirdPhaseAOE_API __declspec(dllimport)
#endif

class ComponentBoxTrigger;
class PlayerMovement;

class ThirdPhaseAOE_API ThirdPhaseAOE : public Script
{
	void Awake() override;
	void Start() override;
	void Update() override;
	void Expose(ImGuiContext* context) override;

	void Serialize(JSON_value* json) const override;
	void DeSerialize(JSON_value* json) override;

	inline virtual ThirdPhaseAOE* Clone() const
	{
		return new ThirdPhaseAOE(*this);
	}
public:
	int aoeType = 0;
	int repetitions = 0;

	void HandleFirstAOE();
	void HandleReplicas();

	//First AOE variables
	float timeToExplodeFirstAOE = 1.5f;
	float timeToFinishFirstAOE = 3.0f;

	GameObject* prepParticlesFirstAOEGO = nullptr;
	GameObject* hitParticlesFirstAOEGO = nullptr;
	GameObject* boxTriggerFirstAOEGO = nullptr;
	ComponentBoxTrigger* boxTriggerComponentFirst = nullptr;

	//second AOE variables
	float timeToExplodeRepetition = 0.5f;
	float timeToFinishRepetition = 0.8f;

	GameObject* prepParticlesSecondAOEGO = nullptr;
	GameObject* hitParticlesSecondAOEGO = nullptr;
	GameObject* boxTriggerSecondAOEGO = nullptr;
	ComponentBoxTrigger* boxTriggerComponentSecond = nullptr;

	GameObject* playerGO = nullptr;
	PlayerMovement* playerScript = nullptr;
};

#endif __ThirdPhaseAOE_h__
