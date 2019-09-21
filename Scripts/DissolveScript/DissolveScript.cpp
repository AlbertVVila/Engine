#include "GameObject.h"
#include "ComponentRenderer.h"

#include "DissolveScript.h"

#include "imgui.h"
#include "JSON.h"	

DissolveScript_API Script* CreateScript()
{
	DissolveScript* instance = new DissolveScript;
	return instance;
}

void DissolveScript::Start()
{
	myRender = gameobject->GetComponent<ComponentRenderer>();
	assert(myRender != nullptr);
}

void DissolveScript::Update()
{

}

void DissolveScript::OnTriggerEnter(GameObject * go)
{
}

void DissolveScript::Expose(ImGuiContext * context)
{
}

void DissolveScript::Serialize(JSON_value * json) const
{
}

void DissolveScript::DeSerialize(JSON_value * json)
{
}
