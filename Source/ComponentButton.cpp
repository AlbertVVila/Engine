#include "Application.h"

#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleUI.h"

#include "ComponentButton.h"
#include "ComponentImage.h"
#include "ComponentText.h"
#include "ComponentTransform2D.h"

#include "GameObject.h"
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
	math::float2 mouse = reinterpret_cast<const float2&>(App->input->GetMousePosition());	
	float screenX = mouse.x - App->renderer->viewGame->winPos.x - (App->ui->currentWidth * .5f);
	float screenY = mouse.y - App->renderer->viewGame->winPos.y - (App->ui->currentHeight * .5f);
	
	float buttonX = rectTransform->position.x;
	float buttonY = rectTransform->position.y;
	
	math::float2 buttonMin = float2(buttonX - rectTransform->size.x *.5f, buttonY - rectTransform->size.y *.5f);
	math::float2 buttonMax = float2(buttonX + rectTransform->size.x *.5f, buttonY + rectTransform->size.y *.5f);
	if (screenX > buttonMin.x && screenX < buttonMax.x && screenY > buttonMin.y && screenY < buttonMax.y)
	{
		isHovered = true;
		buttonImage->enabled = false;
		highlightedImage->enabled = true;
		pressedImage->enabled = false;
	}
	else
	{
		isHovered = false;
		buttonImage->enabled = true;
		highlightedImage->enabled = false;
		pressedImage->enabled = false;
	}

	if (isHovered && App->input->GetMouseButtonDown(1) == KEY_REPEAT)
	{
		isPressed = true;
		buttonImage->enabled = false;
		highlightedImage->enabled = false;
		pressedImage->enabled = true;
	}
}

void ComponentButton::AssemblyButton() 
{
	text->gameobject = gameobject;
	text->text = "Best button EUW";
	buttonImage->gameobject = gameobject;
	highlightedImage->gameobject = gameobject;
	pressedImage->gameobject = gameobject;
	rectTransform = (ComponentTransform2D*)gameobject->GetComponent(ComponentType::Transform2D);
	buttonImage->enabled = true;
	highlightedImage->enabled = false;
	pressedImage->enabled = false;
}