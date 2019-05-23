#include "ComponentBoxTrigger.h"

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
	boxTrigger = new math::OBB();

	boxTrigger->r = size;
	boxTrigger->pos = position;
	boxTrigger->axis[0] = math::float3::unitX;
	boxTrigger->axis[1] = math::float3::unitY;
	boxTrigger->axis[2] = math::float3::unitZ;

	App->collisions->AddBox(this, isPlayer);
}

ComponentBoxTrigger::ComponentBoxTrigger(GameObject * gameobject) : Component(gameobject, ComponentType::BoxTrigger)
{
	boxTrigger = new math::OBB();

	boxTrigger->r = size;
	boxTrigger->pos = position;
	boxTrigger->axis[0] = math::float3::unitX;
	boxTrigger->axis[1] = math::float3::unitY;
	boxTrigger->axis[2] = math::float3::unitZ;

	App->collisions->AddBox(this, isPlayer);
}

ComponentBoxTrigger::ComponentBoxTrigger(const ComponentBoxTrigger & component) : Component(component)
{
	boxTrigger = new math::OBB();

	size = component.size;
	position = component.position;
	isPlayer = component.isPlayer;
	debugDraw = component.debugDraw;

	boxTrigger->r = size;
	boxTrigger->pos = position;
	boxTrigger->axis[0] = component.boxTrigger->axis[0];
	boxTrigger->axis[1] = component.boxTrigger->axis[1];
	boxTrigger->axis[2] = component.boxTrigger->axis[2];

	App->collisions->AddBox(this, isPlayer);
}

ComponentBoxTrigger::~ComponentBoxTrigger()
{
	App->collisions->RemoveBox(this);

	if (boxTrigger != nullptr)
	{
		delete boxTrigger; 
		boxTrigger = nullptr;
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

		bool propIsPlayer = isPlayer;
		if (ImGui::Checkbox("Is Player?", &propIsPlayer)) SetIsPlayer(propIsPlayer);

		ImGui::DragFloat3("Position", position.ptr(), 0.1F, 0.0F, 20.0F);
		if (ImGui::DragFloat3("Size", size.ptr(), 0.1F, 0.0F, 20.0F)) boxTrigger->r = size;

		ImGui::Spacing();

		if (ImGui::CollapsingHeader("Box Corners"))
		{
			ImGui::InputFloat3("0", this->boxTrigger->CornerPoint(0).ptr(), 2, ImGuiInputTextFlags_ReadOnly);
			ImGui::InputFloat3("1", this->boxTrigger->CornerPoint(1).ptr(), 2, ImGuiInputTextFlags_ReadOnly);
			ImGui::InputFloat3("2", this->boxTrigger->CornerPoint(2).ptr(), 2, ImGuiInputTextFlags_ReadOnly);
			ImGui::InputFloat3("3", this->boxTrigger->CornerPoint(3).ptr(), 2, ImGuiInputTextFlags_ReadOnly);
			ImGui::InputFloat3("4", this->boxTrigger->CornerPoint(4).ptr(), 2, ImGuiInputTextFlags_ReadOnly);
			ImGui::InputFloat3("5", this->boxTrigger->CornerPoint(5).ptr(), 2, ImGuiInputTextFlags_ReadOnly);
			ImGui::InputFloat3("6", this->boxTrigger->CornerPoint(6).ptr(), 2, ImGuiInputTextFlags_ReadOnly);
			ImGui::InputFloat3("7", this->boxTrigger->CornerPoint(7).ptr(), 2, ImGuiInputTextFlags_ReadOnly);
		}

		if (ImGui::CollapsingHeader("Contacts"))
		{
			for (auto contact : overlapList) ImGui::Text(contact.first->gameobject->name.c_str());
		}
	}
	ImGui::Separator();
	ImGui::PopID();

}

void ComponentBoxTrigger::Update()
{
	if (!enabled) return;
	boxTrigger->axis[0] = gameobject->transform->right;
	boxTrigger->axis[1] = gameobject->transform->up;
	boxTrigger->axis[2] = gameobject->transform->front;
	boxTrigger->pos = gameobject->transform->global.MulPos(position);

	std::vector<const ComponentBoxTrigger*> toRemove;
	for (auto it = overlapList.begin(); it != overlapList.end(); ++it)
	{
		switch (it->second)
		{
		case OverlapState::Enter:
			it->second = OverlapState::PostIdle;
			PropagateState(it->first->gameobject, OverlapState::Enter);
			break;

		case OverlapState::Idle:
			it->second = OverlapState::PostIdle;
			PropagateState(it->first->gameobject, OverlapState::Idle);
			break;

		case OverlapState::PostIdle:
			it->second = OverlapState::Exit;
			PropagateState(it->first->gameobject, OverlapState::Exit);
			break;

		case OverlapState::Exit:
			toRemove.push_back(it->first);
			break;

		default:
			break;
		}
	}

	for (auto item : toRemove) overlapList.erase(item);

	if (debugDraw) DrawDebug();
}

void ComponentBoxTrigger::OnPlay()
{
	overlapList.clear();
}

void ComponentBoxTrigger::DrawDebug()
{
	ddVec3* corners = new ddVec3[8];
	corners[0] = boxTrigger->CornerPoint(0);
	corners[1] = boxTrigger->CornerPoint(1);
	corners[2] = boxTrigger->CornerPoint(3);
	corners[3] = boxTrigger->CornerPoint(2);
	corners[4] = boxTrigger->CornerPoint(4);
	corners[5] = boxTrigger->CornerPoint(5);
	corners[6] = boxTrigger->CornerPoint(7);
	corners[7] = boxTrigger->CornerPoint(6);

	if (overlapList.size() == 0) dd::box(corners, dd::colors::Cyan);
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
	if (overlapList.find(other) != overlapList.end())
	{
		switch (overlapList[other])
		{
		case OverlapState::Enter:
		case OverlapState::PostIdle:
		case OverlapState::Idle:
			overlapList[other] = OverlapState::Idle;
			return;
			break;

		case OverlapState::Exit:
			overlapList[other] = OverlapState::Enter;
			return;

		default:
			break;
		}
	}

	overlapList[other] = OverlapState::Enter;
}

void ComponentBoxTrigger::RemoveOverlap(const ComponentBoxTrigger * other)
{
	if (overlapList.find(other) != overlapList.end())
	{
		overlapList.erase(other);
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
	debugDraw = (value->GetInt("debug_draw") > 0);

	boxTrigger->r = size;
}

void ComponentBoxTrigger::PropagateState(GameObject * other, OverlapState state)
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
			case OverlapState::Enter:
				((Script*)component)->OnTriggerEnter(other);
				break;
			case OverlapState::Idle:
				((Script*)component)->OnTrigger(other);
				break;
			case OverlapState::Exit:
				((Script*)component)->OnTriggerExit(other);
				break;
			default:
				break;
			}
		}

		go = go->parent;
	}
}

