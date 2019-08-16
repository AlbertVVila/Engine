#ifndef  __RainSkill_h__
#define  __RainSkill_h__

#include "RangeSkill.h"
#include <queue>

class GameObject;

class RainSkill :
	public RangeSkill
{
public:
	RainSkill(PlayerMovement* PM, const char* trigger, const char* prefab);
	~RainSkill();

	void Start() override;

	std::queue<GameObject*> machetes;
	float targetHeight = 0.f;
};

#endif