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
	if (component.script != nullptr)
	{
		SetScript(scriptName);
	}
}

ComponentScript::~ComponentScript()
{
	if (script != nullptr)
	{
		App->scripting->RemoveScript(scriptName, script);
	}
}

ComponentScript * ComponentScript::Clone() const
{
	return new ComponentScript(*this);
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
					SetScript(it->first);
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

void ComponentScript::Save(JSON_value * value) const
{
	Component::Save(value);
	if (script != nullptr)
	{
		value->AddString("script", scriptName.c_str());
	}
}

void ComponentScript::Load(const JSON_value & value)
{
	Component::Load(value);
	const char* retrievedName = value.GetString("script");
	if (retrievedName != nullptr)
	{
		SetScript(retrievedName);
	}
}

void ComponentScript::SetScript(const std::string &name)
{
	if (script != nullptr)
	{
		App->scripting->RemoveScript(scriptName, script);
	}
	script = App->scripting->AddScript(name);
	script->SetApp(App);
	script->SetGameObject(gameobject);
	scriptName = name;
}

