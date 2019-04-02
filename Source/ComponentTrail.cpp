#include "Application.h"

#include "ModuleTime.h"
#include "ModuleParticles.h"

#include "ComponentTrail.h"
#include "ComponentTransform.h"

#include "GameObject.h"

#include "imgui.h"
#include "debugdraw.h"
#include "JSON.h"


ComponentTrail::ComponentTrail(GameObject* gameobject) : Component(gameobject, ComponentType::Trail)
{
	if (!gameobject->transform)
	{
		gameobject->CreateComponent(ComponentType::Transform);
	}
	App->particles->AddTrailRenderer(this);
}

ComponentTrail::ComponentTrail(const ComponentTrail& component) : Component(component)
{	
	if (!gameobject->transform)
	{
		gameobject->CreateComponent(ComponentType::Transform);
	}
	App->particles->AddTrailRenderer(this);
}

void ComponentTrail::Update()
{
	unsigned trailPoints = trail.size();
	for (unsigned i = 0u; i < trailPoints; ++i)
	{
		TrailPoint point = trail.front();
		point.remainingTime -= App->time->gameDeltaTime;

		if (point.remainingTime > 0)
		{
			dd::point(point.position, dd::colors::AntiqueWhite, 10);
			dd::point(point.rightPoint, dd::colors::AntiqueWhite, 2);
			dd::point(point.leftPoint, dd::colors::AntiqueWhite, 2);
			dd::line(point.rightPoint, point.leftPoint, dd::colors::AntiqueWhite);
			trail.push(point);
		}

		trail.pop();
	}

	math::float3 pos = gameobject->transform->GetGlobalPosition();
	if (trail.size() == 0 || trail.back().Distance(pos) > minDistance)
	{
		if (trail.size() == 0)
		{
			TrailPoint newPoint(duration, pos);
			trail.push(newPoint);
		}
		else
		{			
			TrailPoint newPoint(duration, gameobject->transform->GetGlobalPosition(), trail.back().position, width);
			trail.push(newPoint);
		}
	}
}

void ComponentTrail::DrawProperties()
{
	ImGui::PushID(this);
	if (ImGui::CollapsingHeader("Trail Renderer", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::InputFloat("Width", &width, .01f, .1f);
		ImGui::InputFloat("Duration", &duration, .01f, .1f);
		ImGui::InputFloat("Min. point distance", &minDistance, .01f, .1f);
	}
	ImGui::PopID();
}

void ComponentTrail::Save(JSON_value* value) const
{
	Component::Save(value);
	value->AddFloat("width", width);
	value->AddFloat("duration", duration);
	value->AddFloat("minDistance", minDistance);
}

void ComponentTrail::Load(JSON_value* value)
{
	Component::Load(value);
	width = value->GetFloat("width");
	duration = value->GetFloat("duration");
	minDistance = value->GetFloat("minDistance");
}

ComponentTrail * ComponentTrail::Clone() const
{
	return nullptr;
}

ComponentTrail::~ComponentTrail()
{
	App->particles->RemoveTrailRenderer(this);
}
