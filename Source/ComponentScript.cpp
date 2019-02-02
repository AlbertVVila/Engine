#include "ComponentScript.h"
#include "GameObject.h"
#include "JSON.h"
#include "BaseScript.h"
#include "imgui.h"

ComponentScript::ComponentScript(GameObject * gameobject) : Component(gameobject, ComponentType::Script)
{
	//script = CreateScript();
	//script->SetGameObject(gameobject);
}

ComponentScript::ComponentScript(const ComponentScript & component) : Component(component)
{
	script_name = component.script_name;
}

ComponentScript::~ComponentScript()
{
}

ComponentScript * ComponentScript::Clone() const
{
	return new ComponentScript(*this);
}

void ComponentScript::Update()
{
	script->Update();
}

void ComponentScript::DrawProperties()
{
	if (ImGui::CollapsingHeader(script_name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		bool removed = Component::DrawComponentState();
		if (removed)
		{
			return;
		}
		ImGuiContext * context = ImGui::GetCurrentContext();
		script->Expose(context);
	}
}

