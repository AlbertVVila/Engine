#include "ComponentTransform2D.h"
#include "Application.h"
#include "GameObject.h"
#include "ModuleTime.h"
#include "ModuleRender.h"
#include "Viewport.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "JSON.h"

ComponentTransform2D::ComponentTransform2D(GameObject* gameobject) : Component(gameobject, ComponentType::Transform2D)
{
	alignments[TOPLEFT] = math::float2(-1.0f, 1.0f);
	alignments[TOPCENTER] = math::float2(0.0f, 1.0f);
	alignments[TOPRIGHT] = math::float2(1.0f, 1.0f);
	alignments[MIDDLELEFT] = math::float2(-1.0f, 0.0f);
	alignments[MIDDLECENTER] = math::float2(0.0f, 0.0f);
	alignments[MIDDLERIGHT] = math::float2(1.0f, 0.0f);
	alignments[BOTTOMLEFT] = math::float2(-1.0f, -1.0f);
	alignments[BOTTOMCENTER] = math::float2(0.0f, -1.0f);
	alignments[BOTTOMRIGHT] = math::float2(1.0f, -1.0f);
}

ComponentTransform2D::ComponentTransform2D(const ComponentTransform2D & component) : Component(component)
{
	position = component.position;
	size = component.size;
}

ComponentTransform2D::~ComponentTransform2D()
{
}

Component * ComponentTransform2D::Clone() const
{
	return new ComponentTransform2D(*this);
}

void ComponentTransform2D::DrawProperties()
{

	if (ImGui::CollapsingHeader("Rect Transformation", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (gameobject->isStatic && App->time->gameState != GameState::RUN)
		{
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		}

		ImGui::DragFloat2("Position", (float*)&position, 0.1f, -1000.f, 1000.f);

		ImGui::DragFloat2("Size", (float*)&size, 0.5f, 0.f, 1000.f);
	
		if (gameobject->isStatic && App->time->gameState != GameState::RUN)
		{
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
		}


		if (ImGui::CollapsingHeader("Anchor", ImGuiTreeNodeFlags_DefaultOpen))
		{

			if (ImGui::Button("Top Left"))
			{
				float width = (float)App->renderer->viewGame->current_width;
				float height = (float)App->renderer->viewGame->current_height;
				position = math::float2(-width*0.5 + size.x/2, height*0.5 - size.y / 2);
			}
			ImGui::SameLine();
			if (ImGui::Button("Top Center"))
			{
				float height = (float)App->renderer->viewGame->current_height;
				position = math::float2(0.0f, height*0.5 - size.y / 2);
			}
			ImGui::SameLine();
			if (ImGui::Button("Top Right"))
			{
				float width = (float)App->renderer->viewGame->current_width;
				float height = (float)App->renderer->viewGame->current_height;
				position = math::float2(width*0.5 - size.x / 2, height*0.5 - size.y / 2);
			}

			if (ImGui::Button("Middle Left"))
			{
				float width = (float)App->renderer->viewGame->current_width;
				position = math::float2(-width*0.5 + size.x / 2, 0.0f);
			}
			ImGui::SameLine();
			if (ImGui::Button("Middle Center"))
			{
				position = math::float2(0.0f, 0.0f);
			}
			ImGui::SameLine();
			if (ImGui::Button("Middle Right"))
			{
				float width = (float)App->renderer->viewGame->current_width;
				position = math::float2(width*0.5 - size.x / 2, 0.0f);
			}

			if (ImGui::Button("Bottom Left"))
			{
				float width = (float)App->renderer->viewGame->current_width;
				float height = (float)App->renderer->viewGame->current_height;
				position = math::float2(-width*0.5 + size.x / 2,- height*0.5 + size.y / 2);
			}
			ImGui::SameLine();
			if (ImGui::Button("Bottom Center"))
			{
				float height = (float)App->renderer->viewGame->current_height;
				position = math::float2(0.0f, -height*0.5 + size.y / 2);
			}
			ImGui::SameLine();
			if (ImGui::Button("Bottom Right"))
			{
				float width = (float)App->renderer->viewGame->current_width;
				float height = (float)App->renderer->viewGame->current_height;
				position = math::float2(width*0.5 - size.x / 2, -height*0.5 + size.y / 2);
			}
		}
	}
}

void ComponentTransform2D::Save(JSON_value * value) const
{
	Component::Save(value);
	value->AddFloat2("Position", position);
	value->AddFloat2("Size", size);
}

void ComponentTransform2D::Load(JSON_value* value)
{
	Component::Load(value);
	position = value->GetFloat2("Position");
	size = value->GetFloat2("Size");
}