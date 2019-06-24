#pragma once
#include "RangeSkill.h"
class RainSkill :
	public RangeSkill
{
public:
	RainSkill(PlayerMovement* PM, const char* trigger, const char* prefab);
	~RainSkill();

	void Start() override;
};

