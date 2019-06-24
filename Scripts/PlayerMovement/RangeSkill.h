#pragma once
#include "BasicSkill.h"

#include "Math/float3.h"

class RangeSkill :
	public BasicSkill
{
public:
	RangeSkill(PlayerMovement* PM, const char* trigger, const char* prefab);
	virtual ~RangeSkill();

	void Start() override;

protected:
	std::string prefabName;

	math::float3 spawnPosition;
};

