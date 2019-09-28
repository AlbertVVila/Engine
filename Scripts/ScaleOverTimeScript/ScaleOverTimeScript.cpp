#include "ScaleOverTimeScript.h"

#include "Application.h"
#include "ModuleTime.h"

#include "GameObject.h"
#include "ComponentTransform.h"

#include "imgui.h"
#include <assert.h>

ScaleOverTimeScript_API Script* CreateScript()
{
	ScaleOverTimeScript* instance = new ScaleOverTimeScript;
	return instance;
}

void ScaleOverTimeScript::Start()
{
	transform = gameobject->GetComponent<ComponentTransform>();
	if (transform == nullptr)
		LOG("Error: Component transform couldn't be found on %s.", gameobject->name.c_str());
	assert(transform != nullptr);
}

void ScaleOverTimeScript::Update()
{
	if (transform->scale.x < finalScale.x) transform->scale.x += speed * timer;
	if (transform->scale.y < finalScale.y) transform->scale.y += speed * timer;
	if (transform->scale.z < finalScale.z) transform->scale.z += speed * timer;

	timer += App->time->gameDeltaTime;
}

void ScaleOverTimeScript::Expose(ImGuiContext* context)
{
}

void ScaleOverTimeScript::Serialize(JSON_value* json) const
{
}

void ScaleOverTimeScript::DeSerialize(JSON_value* json)
{
}
