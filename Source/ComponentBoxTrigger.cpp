#include "ComponentBoxTrigger.h"

#include "Geometry/OBB.h"

#include "Application.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ModuleCollisions.h"
#include "ModuleTime.h"
#include "BaseScript.h"
#include "JSON.h"
#include "imgui.h"
#include "debugdraw.h"

ComponentBoxTrigger::ComponentBoxTrigger() : Component(nullptr, ComponentType::BoxTrigger)
{
	box_trigger = new math::OBB();

	box_trigger->r = size;
	box_trigger->pos = position;
	box_trigger->axis[0] = math::float3::unitX;
	box_trigger->axis[1] = math::float3::unitY;
	box_trigger->axis[2] = math::float3::unitZ;

	App->collisions->AddBox(this, isPlayer);
}

ComponentBoxTrigger::ComponentBoxTrigger(GameObject * gameobject) : Component(gameobject, ComponentType::BoxTrigger)
{
	box_trigger = new math::OBB();

	box_trigger->r = size;
	box_trigger->pos = position;
	box_trigger->axis[0] = math::float3::unitX;
	box_trigger->axis[1] = math::float3::unitY;
	box_trigger->axis[2] = math::float3::unitZ;

	App->collisions->AddBox(this, isPlayer);
}

ComponentBoxTrigger::ComponentBoxTrigger(const ComponentBoxTrigger & component) : Component(component)
{
	box_trigger = new math::OBB();

	size = component.size;
	position = component.position;
	isPlayer = component.isPlayer;
	debugDraw = component.debugDraw;

	box_trigger->r = size;
	box_trigger->pos = position;
	box_trigger->axis[0] = component.box_trigger->axis[0];
	box_trigger->axis[1] = component.box_trigger->axis[1];
	box_trigger->axis[2] = component.box_trigger->axis[2];

	App->collisions->AddBox(this, isPlayer);
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

void ComponentBoxTrigger::DrawProperties()
{
	ImGui::PushID(this);
	if (ImGui::CollapsingHeader("Box Trigger", ImGuiTreeNodeFlags_DefaultOpen))
	{
		bool removed = Component::DrawComponentState();
		if (removed)
		{
			ImGui::PopID();
			return;
		}

		ImGui::Checkbox("Draw Debug Box", &debugDraw);

		bool prop_is_player = isPlayer;
		if (ImGui::Checkbox("Is Player?", &prop_is_player)) SetIsPlayer(prop_is_player);

		ImGui::DragFloat3("Position", position.ptr(), 0.1F, 0.0F, 20.0F);
		if (ImGui::DragFloat3("Size", size.ptr(), 0.1F, 0.0F, 20.0F)) box_trigger->r = size;

		ImGui::Spacing();

		if (ImGui::CollapsingHeader("Box Corners"))
		{
			ImGui::InputFloat3("0", this->box_trigger->CornerPoint(0).ptr(), 2, ImGuiInputTextFlags_ReadOnly);
			ImGui::InputFloat3("1", this->box_trigger->CornerPoint(1).ptr(), 2, ImGuiInputTextFlags_ReadOnly);
			ImGui::InputFloat3("2", this->box_trigger->CornerPoint(2).ptr(), 2, ImGuiInputTextFlags_ReadOnly);
			ImGui::InputFloat3("3", this->box_trigger->CornerPoint(3).ptr(), 2, ImGuiInputTextFlags_ReadOnly);
			ImGui::InputFloat3("4", this->box_trigger->CornerPoint(4).ptr(), 2, ImGuiInputTextFlags_ReadOnly);
			ImGui::InputFloat3("5", this->box_trigger->CornerPoint(5).ptr(), 2, ImGuiInputTextFlags_ReadOnly);
			ImGui::InputFloat3("6", this->box_trigger->CornerPoint(6).ptr(), 2, ImGuiInputTextFlags_ReadOnly);
			ImGui::InputFloat3("7", this->box_trigger->CornerPoint(7).ptr(), 2, ImGuiInputTextFlags_ReadOnly);
		}

		if (ImGui::CollapsingHeader("Contacts"))
		{
			for (auto contact : overlap_list) ImGui::Text(contact.first->gameobject->name.c_str());
		}
	}
	ImGui::Separator();
	ImGui::PopID();

}

void ComponentBoxTrigger::Update()
{
	if (!enabled) return;
	box_trigger->axis[0] = gameobject->transform->right;
	box_trigger->axis[1] = gameobject->transform->up;
	box_trigger->axis[2] = gameobject->transform->front;
	box_trigger->pos = gameobject->transform->global.MulPos(position);

	std::vector<const ComponentBoxTrigger*> to_remove;
	for (auto it = overlap_list.begin(); it != overlap_list.end(); ++it)
	{
		switch (it->second)
		{
		case Overlap_State::Enter:
			it->second = Overlap_State::PostIdle;
			PropagateState(it->first->gameobject, Overlap_State::Enter);
			break;

		case Overlap_State::Idle:
			it->second = Overlap_State::PostIdle;
			PropagateState(it->first->gameobject, Overlap_State::Idle);
			break;

		case Overlap_State::PostIdle:
			it->second = Overlap_State::Exit;
			PropagateState(it->first->gameobject, Overlap_State::Exit);
			break;

		case Overlap_State::Exit:
			to_remove.push_back(it->first);
			break;

		default:
			break;
		}
	}

	for (auto item : to_remove) overlap_list.erase(item);

	if (debugDraw) DrawDebug();
}

void ComponentBoxTrigger::OnPlay()
{
	overlap_list.clear();
}

void ComponentBoxTrigger::DrawDebug()
{
	ddVec3* corners = new ddVec3[8];
	corners[0] = box_trigger->CornerPoint(0);
	corners[1] = box_trigger->CornerPoint(1);
	corners[2] = box_trigger->CornerPoint(3);
	corners[3] = box_trigger->CornerPoint(2);
	corners[4] = box_trigger->CornerPoint(4);
	corners[5] = box_trigger->CornerPoint(5);
	corners[6] = box_trigger->CornerPoint(7);
	corners[7] = box_trigger->CornerPoint(6);

	if (overlap_list.size() == 0) dd::box(corners, dd::colors::Cyan);
	else dd::box(corners, dd::colors::Salmon);
	delete[] corners;
}

void ComponentBoxTrigger::SetIsPlayer(bool isPlayer)
{
	this->isPlayer = isPlayer;
	App->collisions->AddBox(this, isPlayer);
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

void ComponentBoxTrigger::Save(JSON_value * value) const
{
	Component::Save(value);
	value->AddFloat3("pos", position);
	value->AddFloat3("r", size);
	value->AddInt("is_player", isPlayer ? 1 : 0);
	value->AddInt("debug_draw", debugDraw ? 1 : 0);
}

void ComponentBoxTrigger::Load(JSON_value * value)
{
	Component::Load(value);
	position = value->GetFloat3("pos");
	size = value->GetFloat3("r");
	isPlayer = (value->GetInt("is_player") > 0);
	isPlayer = (value->GetInt("debug_draw") > 0);

	box_trigger->r = size;
}

void ComponentBoxTrigger::PropagateState(GameObject * other, Overlap_State state)
{
	if (App->time->gameState != GameState::RUN) return;

	GameObject* go = this->gameobject;

	while (go != nullptr)
	{
		for (auto component : go->components)
		{
			if (component->type != ComponentType::Script) continue;

			switch (state)
			{
			case Overlap_State::Enter:
				((Script*)component)->OnTriggerEnter(other);
				break;
			case Overlap_State::Idle:
				((Script*)component)->OnTrigger(other);
				break;
			case Overlap_State::Exit:
				((Script*)component)->OnTriggerExit(other);
				break;
			default:
				break;
			}
		}

		go = go->parent;
	}
}

