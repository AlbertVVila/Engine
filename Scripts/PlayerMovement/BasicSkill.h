#ifndef  __BasicSkill_h__
#define  __BasicSkill_h__

#include "imgui.h"
#include <string>

class PlayerMovement;
class JSON_value;

class BasicSkill
{
public:
	BasicSkill(PlayerMovement* PM, const char* trigger);
	virtual ~BasicSkill();

	virtual void Start();
	virtual void Update();
	virtual void Exit();

	virtual void Reset();

	virtual void Expose(ImGuiContext* context);
	virtual void Serialize(JSON_value* json) const;
	virtual void DeSerialize(JSON_value* json);

	virtual void CheckInput() {};

protected:
	virtual void UseSkill() {}

public:
	std::string animTrigger;
	float duration = 0.0f;

protected:
	float timer = 0.0f;
	PlayerMovement* player = nullptr;
};

#endif __BasicSkill_h__