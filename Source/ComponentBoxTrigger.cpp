#include "ComponentBoxTrigger.h"

#include "Geometry/OBB.h"

#include "Application.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ModuleCollisions.h"

ComponentBoxTrigger::ComponentBoxTrigger() : Component(nullptr, ComponentType::BoxTrigger)
{
	box_trigger = new math::OBB();

	box_trigger->r = float3::one;
	box_trigger->pos = float3::zero;
}

ComponentBoxTrigger::ComponentBoxTrigger(GameObject * gameobject) : Component(gameobject, ComponentType::BoxTrigger)
{
	box_trigger = new math::OBB();

	box_trigger->r   = float3::one;
	box_trigger->pos = float3::zero;
}

ComponentBoxTrigger::ComponentBoxTrigger(const ComponentBoxTrigger & component) : Component(component)
{
	box_trigger = new math::OBB();

	box_trigger->r = float3::one;
	box_trigger->pos = float3::zero;
}

ComponentBoxTrigger::~ComponentBoxTrigger()
{
	App->collisions->RemoveBox(this);

	if (box_trigger != nullptr)
	{
		delete box_trigger; 
		box_trigger = nullptr;
	}
}

Component * ComponentBoxTrigger::Clone() const
{
	return new ComponentBoxTrigger(*this);
}

void ComponentBoxTrigger::Update()
{
	box_trigger->Transform(gameobject->transform->global);

	std::vector<const ComponentBoxTrigger*> to_remove;
	for (auto it = overlap_list.begin(); it != overlap_list.end(); ++it)
	{
		switch (it->second)
		{
		case Overlap_State::Enter:
			it->second = Overlap_State::PostIdle;
			// Todo Propagate Enter
			break;

		case Overlap_State::Idle:
			it->second = Overlap_State::PostIdle;
			// Todo: Propagate Idle
			break;

		case Overlap_State::PostIdle:
			it->second = Overlap_State::Exit;
			// Todo Propagate Exit
			break;

		case Overlap_State::Exit:
			to_remove.push_back(it->first);
			break;

		default:
			break;
		}
	}

	for (auto item : to_remove) overlap_list.erase(item);
}

void ComponentBoxTrigger::SetIsPlayer(bool is_player)
{
	this->is_player = is_player;
	App->collisions->AddBox(this, is_player);
}

void ComponentBoxTrigger::AddOverlap(const ComponentBoxTrigger * other)
{
	if (overlap_list.find(other) != overlap_list.end())
	{
		switch (overlap_list[other])
		{
		case Overlap_State::Enter:
		case Overlap_State::PostIdle:
		case Overlap_State::Idle:
			overlap_list[other] = Overlap_State::Idle;
			return;
			break;

		case Overlap_State::Exit:
			overlap_list[other] = Overlap_State::Enter;
			return;

		default:
			break;
		}
	}

	overlap_list[other] = Overlap_State::Enter;
}

void ComponentBoxTrigger::RemoveOverlap(const ComponentBoxTrigger * other)
{
	if (overlap_list.find(other) != overlap_list.end())
	{
		overlap_list.erase(other);
	}
}

