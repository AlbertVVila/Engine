#include "ComponentText.h"
#include "GameObject.h"
#include "JSON.h"
#include "Imgui/imgui.h"
#include "Application.h"
#include "ModuleFontLoader.h"

ComponentText::ComponentText() : Component(nullptr, ComponentType::Text)
{
	font = App->fonts->defaultFont;
}

ComponentText::ComponentText(GameObject* gameobject) : Component(gameobject, ComponentType::Text)
{
	font = App->fonts->defaultFont;
}

ComponentText::ComponentText(const ComponentText &copy) : Component(copy)
{
	//copy all the shiet madafaka
}

ComponentText::~ComponentText()
{
}


void ComponentText::Update()
{
	if (enabled)
	{

	}
}
void ComponentText::DrawProperties()
{

}
void ComponentText::Save(JSON_value *value)const
{
	Component::Save(value);
	value->AddFloat("FontSize", fontSize);
	value->AddString("text", text);
	value->AddString("text", text);
}
void ComponentText::Load(const JSON_value &value)
{
	Component::Load(value);
	fontSize = value.GetFloat("FontSize");
	text = value.GetString("text");
}