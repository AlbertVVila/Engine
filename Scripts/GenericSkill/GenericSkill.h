#ifndef  __GenericSkill_h__
#define  __GenericSkill_h__

#include "BaseScript.h"

class PlayerMovement;

class GenericSkill : public Script
{
public:
	GenericSkill(PlayerMovement* PM);
	~GenericSkill();

	void Update() override;

	void Expose(ImGuiContext* context) override;
	void Serialize(JSON_value* json) const override;
	void DeSerialize(JSON_value* json) override;

protected:
	void UseSkill() {}
	void Reset();

public:
	float speed = 1.0f;
	float duration = 1.0f;

protected:
	float timer = 0.0f;

	PlayerMovement* player = nullptr;
};

#endif __GenericSkill_h__