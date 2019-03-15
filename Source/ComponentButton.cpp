#include "Application.h"

#include "ModuleInput.h"
#include "ModuleRender.h"


#include "ComponentButton.h"
#include "ComponentImage.h"
#include "ComponentText.h"

#include "Viewport.h"
#include "imgui.h"
#include "MathGeoLib/include/Math/float2.h"


ComponentButton::ComponentButton() : Component(nullptr, ComponentType::Button)
{
	text = new ComponentText();
	buttonImage = new ComponentImage();
	highlightedImage = new ComponentImage();
	pressedImage = new ComponentImage();
	AssemblyButton();
}

ComponentButton::ComponentButton(GameObject* gameobject) : Component(gameobject, ComponentType::Image)
{
	text = new ComponentText();
	buttonImage = new ComponentImage();
	highlightedImage = new ComponentImage();
	pressedImage = new ComponentImage();
	AssemblyButton();
}

ComponentButton::ComponentButton(const ComponentButton& copy) : Component(copy)
{
	text = (ComponentText*) copy.text->Clone();
	buttonImage = (ComponentImage*)copy.buttonImage->Clone();
	highlightedImage = (ComponentImage*)copy.highlightedImage->Clone();
	pressedImage = (ComponentImage*)copy.pressedImage->Clone();
	AssemblyButton();
}


ComponentButton::~ComponentButton()
{
}

Component * ComponentButton::Clone() const
{
	return nullptr;
}

void ComponentButton::DrawProperties()
{
	ImGui::PushID(0);
	text->DrawProperties();
	ImGui::Text("Button image");
	ImGui::PushID(1);
	buttonImage->DrawProperties();	
	ImGui::Text("Highlighted image");
	ImGui::PushID(2);
	highlightedImage->DrawProperties();
	ImGui::Text("Pressed image");
	ImGui::PushID(3);
	pressedImage->DrawProperties();
	ImGui::PopID();
	ImGui::PopID();
	ImGui::PopID();
	ImGui::PopID();
}

void ComponentButton::Save(JSON_value * value) const
{
}

void ComponentButton::Load(JSON_value * value)
{
}

void ComponentButton::Update()
{
	math::float2 mousePos = reinterpret_cast<const float2&>(App->input->GetMousePosition());
	LOG("Mouse x %.3f y %.3f", mousePos.x, mousePos.y);
}

void ComponentButton::AssemblyButton() 
{
	text->gameobject = gameobject;
	text->text = "Best button EUW";
	buttonImage->gameobject = gameobject;
	highlightedImage->gameobject = gameobject;
	pressedImage->gameobject = gameobject;
}