#ifndef  __RainSkillSpawn_h__
#define  __RainSkillSpawn_h__

#include "BaseScript.h"

#ifdef RainSkillSpawn_EXPORTS
#define RainSkillSpawn_API __declspec(dllexport)
#else
#define RainSkillSpawn_API __declspec(dllimport)
#endif

class JSON_value;
class ComponentBoxTrigger;
class GameObject;

class RainSkillSpawn_API RainSkillSpawn : public Script
{
public:
	void Start() override;
	void Update() override;

	void Expose(ImGuiContext* context) override;

	void Serialize(JSON_value* json) const override;
	void DeSerialize(JSON_value* json) override;

	void OnTriggerEnter(GameObject* go);

	inline virtual RainSkillSpawn* Clone() const { return new RainSkillSpawn(*this); }

private:
	void Rain();
	void Reset();

public:
	int hits = 5;				// How many times the hitbox hits?
	float duration = 4.0f;		// How much time the rain will last (seconds) 

private:
	float timer = 0.0f;
	float hitTimer = 0.0f;		// Timer to check time performing a hit

	float hitDuration = 0.0f;	// Time between the hitbox is enabled and disabled (calculated at start)
	ComponentBoxTrigger* attackBoxTrigger;

	bool start = false;
};

extern "C" RainSkillSpawn_API Script* CreateScript();

#endif __RainSkillSpawn_h__
