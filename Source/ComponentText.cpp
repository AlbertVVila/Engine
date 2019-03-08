#include "ComponentText.h"
#include "GameObject.h"
#include "JSON.h"
#include "Imgui/imgui.h"
#include "Application.h"
#include "ModuleFontLoader.h"
#include <vector>
#define MAX_TEXT_LENGTH 64
#define MAX_FONT_SIZE 64

ComponentText::ComponentText() : Component(nullptr, ComponentType::Text)
{
	font = App->fontLoader->defaultFont;
	App->fontLoader->texts.push_back(this);
}

ComponentText::ComponentText(GameObject* gameobject) : Component(gameobject, ComponentType::Text)
{
	font = App->fontLoader->defaultFont;
	App->fontLoader->texts.push_back(this);
}

ComponentText::ComponentText(const ComponentText &copy) : Component(copy)
{
	//copy all the shiet madafaka
	font = copy.font;
	fontSize = copy.fontSize;
	text = copy.text;
	color = copy.color;
	App->fontLoader->texts.push_back(this);
}

ComponentText::~ComponentText()
{

}

Component * ComponentText::Clone() const
{
	return new ComponentText(*this);
}

void ComponentText::DrawProperties()
{
	if (ImGui::CollapsingHeader("Text", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("Enabled", &enabled);
		
		//text value
		char* text_value = new char[MAX_TEXT_LENGTH];
		strcpy(text_value, text);
		ImGui::InputText("##", text_value, MAX_TEXT_LENGTH);
		text = text_value;
		delete[] text_value;

		//font selector
		if (ImGui::BeginCombo("Font", font))
		{
			for (std::map<const char*, std::vector<ModuleFontLoader::Character>>::iterator it = App->fontLoader->fonts.begin(); 
				it != App->fontLoader->fonts.end(); ++it)
			{
				bool is_selected = (*it).first == font;
				if (ImGui::Selectable((*it).first, is_selected))
				{
					font = (*it).first;
				}
				if (is_selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		//size
		ImGui::DragFloat("Font size", &fontSize, 1.0f, 1.0f, MAX_FONT_SIZE);

		//color
		ImGui::ColorEdit3("Font color", (float*)&color);
		
		ImGui::Separator();
	}
}
void ComponentText::Save(JSON_value *value)const
{
	Component::Save(value);
	value->AddFloat("FontSize", fontSize);
	value->AddString("text", text);
	value->AddString("font", font);
	value->AddFloat3("color", color);
}
void ComponentText::Load(const JSON_value &value)
{
	Component::Load(value);
	fontSize = value.GetFloat("FontSize");
	text = value.GetString("text");
	font = value.GetString("font");
	color = value.GetFloat3("color");
}