#ifndef __Module_Collisions_h__
#define __Module_Collisions_h__

#include "Module.h"
#include <vector>

class ComponentBoxTrigger;
enum class BoxTriggerType;

struct OverlapPair
{
	OverlapPair(ComponentBoxTrigger* player, ComponentBoxTrigger* other) : player(player), other(other) {}

	ComponentBoxTrigger* player = nullptr;
	ComponentBoxTrigger* other  = nullptr;
};

class ModuleCollisions : public Module
{
public:
	virtual update_status Update(float dt) override;

	void AddBox(ComponentBoxTrigger* box, BoxTriggerType isPlayer);
	bool RemoveBox(ComponentBoxTrigger* box);

private:
	std::vector<ComponentBoxTrigger*> playerHpBoxes;
	std::vector<ComponentBoxTrigger*> playerAttackBoxes;
	std::vector<ComponentBoxTrigger*> enemyHpBoxes;
	std::vector<ComponentBoxTrigger*> enemyAttackBoxes;
};

#endif // !__Module_Collisions_h__

