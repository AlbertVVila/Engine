#ifndef  __BasicSkill_h__
#define  __BasicSkill_h__

#include "imgui.h"

class PlayerMovement;
class JSON_value;

class BasicSkill
{
public:
	BasicSkill(PlayerMovement* PM);
	virtual ~BasicSkill();

	virtual void Start();
	virtual void Update();

	virtual void Expose(ImGuiContext* context);
	virtual void Serialize(JSON_value* json) const;
	virtual void DeSerialize(JSON_value* json);

protected:
	virtual void UseSkill() {}
	virtual void Reset();

public:
	//float speed = 1.0f;
	float duration = 1.0f;

protected:
	float timer = 0.0f;

protected:
	PlayerMovement* player = nullptr;
};

#endif __BasicSkill_h__