#include "Application.h"

#include "ModuleScript.h"
#include "ComponentScript.h"

#include "GameObject.h"
#include "BaseScript.h"
#include "JSON.h"
#include "imgui.h"

#include <map>

ComponentScript::ComponentScript(GameObject * gameobject) : Component(gameobject, ComponentType::Script)
{
}

ComponentScript::ComponentScript(const ComponentScript & component) : Component(component)
{
	scriptName = component.scriptName;
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
}

void ComponentScript::DrawProperties()
{
	if (ImGui::CollapsingHeader(scriptName.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		bool removed = Component::DrawComponentState();
		if (removed)
		{
			return;
		}

		ImGui::Text("Select Script");
		if (ImGui::BeginCombo("", scriptName.c_str()))
		{
			std::map<std::string, int>::const_iterator it;
			for (it = App->scripting->scripts.begin(); it != App->scripting->scripts.end(); it++)
			{
				bool is_selected = (scriptName == it->first);
				if (ImGui::Selectable(it->first.c_str(), is_selected) && scriptName != it->first)
				{
					script = App->scripting->AddScript(it->first);
					script->SetGameObject(gameobject);
					scriptName = it->first;
				}
				if (is_selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		ImGuiContext * context = ImGui::GetCurrentContext();
		if (script != nullptr)
		{
			script->Expose(context);
		}
	}
}

