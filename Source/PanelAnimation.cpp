#include "Application.h"

#include "ModuleScene.h"

#include "GameObject.h"
#include "ComponentAnimation.h"
#include "ComponentTransform.h"
#include "PanelAnimation.h"
#include "imgui.h"
#include "Globals.h"

PanelAnimation::PanelAnimation()
{
}


PanelAnimation::~PanelAnimation()
{
}

void PanelAnimation::Draw()
{
	if (!ImGui::Begin("Animation", &enabled))
	{
		ImGui::End();
		return;
	}
	if (App->scene->selected != nullptr && App->scene->selected->isBoneRoot)
	{
		Animation* anim = ((ComponentAnimation*)(App->scene->selected->GetComponent(ComponentType::Animation)))->anim;
		ComponentAnimation* compAnim = ((ComponentAnimation*)(App->scene->selected->GetComponent(ComponentType::Animation)));

		ImGui::Text("GAMEOBJECT");
		ImGui::Separator();
		ImGui::Text(App->scene->selected->name.c_str());

		ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 - ImGui::CalcTextSize("FRAMES").x / 2);
		ImGui::Text("FRAMES");

		ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 5.7f);
		if (ImGui::SliderInt("##label", &anim->currentFrame, 0, anim->duration))
		{
			UpdateGameObjectAnimation(App->scene->selected, anim);
		}

		ImGui::SameLine(ImGui::GetWindowWidth() - ImGui::GetWindowWidth() / 6);

		if (ImGui::Button("Clip", ImVec2(60, 23)))
		{
			if (!isCliping && anim->currentFrame + 1 <= anim->duration)
			{
				minFrame = anim->currentFrame;
				anim->currentFrame++;
				isCliping = true;
			}
			else
			{
				isCliping = false;
			}
		}

		if (isCliping)
		{
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				isCliping = false;
			}
		}

		ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 - 43);
		if (ImGui::Button("<<", ImVec2(23, 23)))
		{
			if (isCliping && anim->currentFrame - 1 > minFrame)
			{
				anim->currentFrame--;
				UpdateGameObjectAnimation(App->scene->selected, anim);
			}
			else if (!isCliping && anim->currentFrame > 0)
			{
				anim->currentFrame--;
				UpdateGameObjectAnimation(App->scene->selected, anim);
			}

		}
		ImGui::SameLine();

		if (compAnim->isPlaying)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, { 0.5f, 0.3f, 0.3f, 0.7f });

			if (ImGui::ArrowButton("play", ImGuiDir_Right))
			{
				if (compAnim->isPlaying)
					compAnim->isPlaying = false;
				else
					compAnim->isPlaying = true;
			}

			ImGui::PopStyleColor();
		}
		else
		{
			if (ImGui::ArrowButton("play", ImGuiDir_Right))
			{
				if (compAnim->isPlaying)
					compAnim->isPlaying = false;
				else
					compAnim->isPlaying = true;
			}
		}

		ImGui::SameLine();
		if (ImGui::Button(">>", ImVec2(23, 23)))
		{
			if (anim->currentFrame < anim->duration)
			{
				anim->currentFrame++;
				UpdateGameObjectAnimation(App->scene->selected, anim);
			}
		}

		if (isCliping)
		{
			ImGui::SameLine(ImGui::GetWindowWidth() - ImGui::GetWindowWidth() / 6);
			ImGui::Button(std::to_string(minFrame).c_str(), ImVec2(60, 23));
			ImGui::SameLine(); ImGui::Text("Frame Start");

			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - ImGui::GetWindowWidth() / 6);
			ImGui::Button(std::to_string(anim->currentFrame).c_str(), ImVec2(60, 23));
			ImGui::SameLine(); ImGui::Text("Frame End");

			if (anim->currentFrame < minFrame)
			{
				anim->currentFrame = minFrame + 1;
			}
		}

	}

	ImGui::End();
}

void PanelAnimation::UpdateGameObjectAnimation(GameObject * go, Animation* anim)
{
	if (go->animationIndexChannel != 999u)
	{
		go->transform->SetPosition(anim->GetPosition(go->animationIndexChannel, anim->currentFrame));
		go->transform->SetRotation(anim->GetRotation(go->animationIndexChannel, anim->currentFrame));
	}

	for (auto child : go->children)
	{
		UpdateGameObjectAnimation(child, anim);
	}
}
