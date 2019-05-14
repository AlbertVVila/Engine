#include "ModuleCollisions.h"

#include "ComponentBoxTrigger.h"
#include "Geometry/OBB.h"

update_status ModuleCollisions::Update(float dt)
{
	for (auto player : player_boxes)
	{
		const math::OBB* player_obb = player->GetOBB();
		for (auto other : other_boxes)
		{
			const math::OBB* other_obb = other->GetOBB();
			if (player_obb->Intersects(*other_obb))
			{
				player->AddOverlap(other);
				other->AddOverlap(player);
			}
		}
	}

	return update_status::UPDATE_CONTINUE;
}

void ModuleCollisions::AddBox(ComponentBoxTrigger* box, bool is_player)
{
	RemoveBox(box);
	
	if (is_player) player_boxes.push_back(box);
	else player_boxes.push_back(box);
}

bool ModuleCollisions::RemoveBox(ComponentBoxTrigger* box)
{
	for (size_t i = player_boxes.size() - 1; i >= 0; --i)
	{
		if (player_boxes[i] == box)
		{
			player_boxes.erase(player_boxes.begin() + i);
			return true;
		}
	}
	for (size_t i = other_boxes.size() - 1; i >= 0; --i)
	{
		if (other_boxes[i] == box)
		{
			other_boxes.erase(other_boxes.begin() + i);
			return true;
		}
	}

	return false;
}
