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
	if (transform->scale.x < finalScale.x) transform->scale.x += speed * timer;
	if (transform->scale.y < finalScale.y) transform->scale.y += speed * timer;
	if (transform->scale.z < finalScale.z) transform->scale.z += speed * timer;

	timer += App->time->gameDeltaTime;
}

void ScaleOverTime::Expose(ImGuiContext* context)
{
}

void ScaleOverTime::Serialize(JSON_value* json) const
{
}

void ScaleOverTime::DeSerialize(JSON_value* json)
{
}