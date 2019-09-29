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
	timer = App->time->gameDeltaTime;
	math::float3 newScale = transform->scale;

	if (transform->scale.x < finalScale.x) newScale.x += speed * timer;
	else if (loop) newScale.x = startScale.x;
	else newScale.x = finalScale.x;
		

	if (transform->scale.y < finalScale.y) newScale.y += speed * timer;
	else if (loop) newScale.y = startScale.y;
	else newScale.y = finalScale.y;

	if (transform->scale.z < finalScale.z) newScale.z += speed * timer;
	else if (loop) newScale.z = startScale.z;
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
}

void ScaleOverTime::Serialize(JSON_value* json) const
{
	assert(json != nullptr);
	json->AddFloat("speed", speed);
	json->AddFloat3("finalScale", finalScale);
	json->AddInt("loop", loop);
}

void ScaleOverTime::DeSerialize(JSON_value* json)
{
	assert(json != nullptr);
	speed = json->GetFloat("speed", 0.1f);
	finalScale = json->GetFloat3("finalScale");
	loop = json->GetInt("loop");
}

