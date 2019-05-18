#ifndef __Module_Collisions_h__
#define __Module_Collisions_h__

#include "Module.h"
#include <vector>

class ComponentBoxTrigger;

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

	void AddBox(ComponentBoxTrigger* box, bool is_player);
	bool RemoveBox(ComponentBoxTrigger* box);

private:
	std::vector<ComponentBoxTrigger*> player_boxes;
	std::vector<ComponentBoxTrigger*> other_boxes;
};

#endif // !__Module_Collisions_h__

