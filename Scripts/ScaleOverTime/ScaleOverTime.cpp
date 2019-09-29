#include "ScaleOverTime.h"

#include "Application.h"
#include "ModuleTime.h"

#include "GameObject.h"
#include "ComponentTransform.h"

#include "imgui.h"
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
		LOG("Error: Component transform couldn't be found on %s.", gameobject->name.c_str());
	assert(transform != nullptr);
}

void ScaleOverTime::Update()
{
	timer += App->time->gameDeltaTime;
	math::float3 newScale = transform->scale;

	if (transform->scale.x < finalScale.x) 
		newScale.x += speed * timer;
	if (transform->scale.y < finalScale.y) 
		newScale.y += speed * timer;
	if (transform->scale.z < finalScale.z) 
		newScale.z += speed * timer;

	transform->SetScale(newScale);
}

void ScaleOverTime::Expose(ImGuiContext* context)
{
	ImGui::DragFloat("Speed", &speed);
	ImGui::DragFloat3("Final Scale", (float*)&finalScale);
}

void ScaleOverTime::Serialize(JSON_value* json) const
{
}

void ScaleOverTime::DeSerialize(JSON_value* json)
{
}