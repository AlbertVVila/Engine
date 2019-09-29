#include "ScaleOverTime.h"

#include "Application.h"
#include "ModuleTime.h"

#include "GameObject.h"
#include "ComponentTransform.h"

#include "imgui.h"
#include "JSON.h"
#include <assert.h>

ScaleOverTime_API Script* CreateScript()
{
	ScaleOverTime* instance = new ScaleOverTime;
	return instance;
}

void ScaleOverTime::Start()
{
	transform = gameobject->GetComponent<ComponentTransform>();
	if (transform == nullptr)
	{
		LOG("Error: Component transform couldn't be found on %s.", gameobject->name.c_str());
	}
	else
	{
		startScale = transform->scale;
	}

	assert(transform != nullptr);
}

void ScaleOverTime::Update()
{
	if(acceleration)
		timer += App->time->gameDeltaTime;
	else
		timer = App->time->gameDeltaTime;

	math::float3 newScale = transform->scale;

	// X
	if (transform->scale.x < finalScale.x) newScale.x += speed * timer;
	else if (loop)
	{
		newScale.x = startScale.x;
		timer = 0.0f;
	}
	else newScale.x = finalScale.x;
		
	// Y
	if (transform->scale.y < finalScale.y) newScale.y += speed * timer;
	else if (loop)
	{
		newScale.y = startScale.y;
		timer = 0.0f;
	}
	else newScale.y = finalScale.y;

	// Z
	if (transform->scale.z < finalScale.z) newScale.z += speed * timer;
	else if (loop)
	{
		newScale.z = startScale.z;
		timer = 0.0f;
	}
	else newScale.z = finalScale.z;

	if (finalScale != newScale)
		transform->SetScale(newScale);
	else
		this->Enable(false);
}

void ScaleOverTime::Expose(ImGuiContext* context)
{
	ImGui::DragFloat("Speed", &speed, 0.01f);
	ImGui::DragFloat3("Final Scale", (float*)&finalScale);
	ImGui::Checkbox("Loop", &loop);
	if (ImGui::Checkbox("Acceleration", &acceleration))
	{
		timer = 0.0f;
	}
}

void ScaleOverTime::Serialize(JSON_value* json) const
{
	assert(json != nullptr);
	json->AddFloat("speed", speed);
	json->AddFloat3("finalScale", finalScale);
	json->AddInt("loop", loop);
	json->AddInt("acceleration", acceleration);
}

void ScaleOverTime::DeSerialize(JSON_value* json)
{
	assert(json != nullptr);
	speed = json->GetFloat("speed", 0.1f);
	finalScale = json->GetFloat3("finalScale");
	loop = json->GetInt("loop");
	acceleration = json->GetInt("acceleration", 1);
}

