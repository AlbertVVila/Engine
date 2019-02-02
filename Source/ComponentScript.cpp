#include "Application.h"

#include "ModuleScript.h"
#include "ComponentScript.h"

#include "GameObject.h"
#include "BaseScript.h"
#include "JSON.h"
#include "imgui.h"

ComponentScript::ComponentScript(GameObject * gameobject) : Component(gameobject, ComponentType::Script)
{
	//script = CreateScript();
	//script->SetGameObject(gameobject);
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
			for (int n = 0; n < App->scripting->scripts.size(); n++)
			{
				bool is_selected = (scriptName == App->scripting->scripts[n]);
				if (ImGui::Selectable(App->scripting->scripts[n].c_str(), is_selected) && scriptName != App->scripting->scripts[n])
				{
					script = App->scripting->AddScript(App->scripting->scripts[n]);
					scriptName = App->scripting->scripts[n];
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

