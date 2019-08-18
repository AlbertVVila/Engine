#ifndef  __RainSkill_h__
#define  __RainSkill_h__

#include "RangeSkill.h"
#include <vector>

#define AREA_SIZE 200.f
#define VERTICAL_OFFSET 300.f

class GameObject;
class ComponentRenderer;
class ComponentBoxTrigger;

class RainSkill :
	public RangeSkill
{
public:

	struct MacheteUnit 
	{
		GameObject* machete = nullptr;
		ComponentRenderer* renderer = nullptr;
		ComponentBoxTrigger* trigger = nullptr;
		math::float3 originalScale;
		bool landed = false;
	};
	RainSkill(PlayerMovement* PM, const char* trigger, const char* prefab);
	~RainSkill();

	void Start() override;
	void Prepare() override;
	std::vector<MacheteUnit> machetes;

	float targetHeight = 0.f;
};

#endif