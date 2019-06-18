#ifndef  __GenericSkill_h__
#define  __GenericSkill_h__

#include "BaseScript.h"

class PlayerMovement;

class GenericSkill : public Script
{
public:
	GenericSkill(PlayerMovement* PM);
	virtual ~GenericSkill();

	virtual void Start() override;
	virtual void Update() override;

	virtual void Expose(ImGuiContext* context) override;
	virtual void Serialize(JSON_value* json) const override;
	virtual void DeSerialize(JSON_value* json) override;

protected:
	virtual void UseSkill() {}
	virtual void Reset();

public:
	//float speed = 1.0f;
	float duration = 1.0f;

protected:
	float timer = 0.0f;

	PlayerMovement* player = nullptr;
};

#endif __GenericSkill_h__