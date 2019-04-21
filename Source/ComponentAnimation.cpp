#include "Application.h"

#include "ModuleResourceManager.h"
#include "ModuleFileSystem.h"
#include "ModuleTime.h"

#include "GameObject.h"
#include "Resource.h"
#include "ResourceAnimation.h"
#include "ResourceStateMachine.h"
#include "AnimationController.h"
#include "ComponentAnimation.h"
#include "ComponentTransform.h"

#include "Globals.h"
#include "HashString.h"
#include "Imgui/include/imgui.h"
#include "JSON.h"
#include "Math/Quat.h"
#include "Math/float3.h"
#include "Brofiler.h"

ComponentAnimation::ComponentAnimation() : Component(nullptr, ComponentType::Animation)
{
	controller = new AnimationController();
	PlayAnimation(100u);
}

ComponentAnimation::ComponentAnimation(GameObject * gameobject) : Component(gameobject, ComponentType::Animation)
{
	//anim = new ResourceAnimation();
	controller = new AnimationController();
	PlayAnimation(100u);
}

ComponentAnimation::~ComponentAnimation()
{
	delete controller;
	controller = nullptr;
	if (context)
	{
		ax::NodeEditor::DestroyEditor(context);
		context = nullptr;
	}
	anim = nullptr;
	gameobject->isBoneRoot = false;
	RELEASE_ARRAY(animName);
}


void ComponentAnimation::DrawProperties()
{
	ImGui::PushID(this);
	if (ImGui::CollapsingHeader("Animation", ImGuiTreeNodeFlags_DefaultOpen))
	{
		bool removed = Component::DrawComponentState();
		if (removed)
		{
			ImGui::PopID();
			return;
		}

		if (ImGui::Button("New State Machine"))
		{
			if (stateMachine != nullptr)
				stateMachine->Save();

			stateMachine = (ResourceStateMachine*)App->resManager->CreateNewResource(TYPE::STATEMACHINE);
		}
	
		if (stateMachine != nullptr)
		{
			ImGui::SameLine();

			ImGui::PushID("State Machine Combo");
			if (ImGui::BeginCombo("SM", stateMachine != nullptr ? stateMachine->name.c_str() : ""))
			{
				if (guiStateMachines.empty())
				{
					guiStateMachines = App->resManager->GetSMNamesList(true);
				}
				for (unsigned i = 0u; i < guiStateMachines.size(); i++)
				{
					bool is_selected = (stateMachine != nullptr ? stateMachine->name == guiStateMachines[i] : false);
					if (ImGui::Selectable(guiStateMachines[i].c_str(), is_selected))
					{
						stateMachine->Save();
						SetStateMachine(guiStateMachines[i].c_str());
					}
					if (is_selected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
			else
			{
				guiStateMachines.clear();
			}
			ImGui::PopID();


			//Here we should have the name of the stateMachine
			char* smName = new char[MAX_CLIP_NAME];
			strcpy(smName, stateMachine->name.c_str());
			ImGui::InputText("SM name", smName, MAX_CLIP_NAME);
			stateMachine->name = smName;

			if (ImGui::Button("AddClip"))
			{
				stateMachine->AddClip(HashString("Clippity clip"), 0u, true);
				stateMachine->Save();
			}

			if (!stateMachine->isClipsEmpty())
			{
				ImGui::Separator();
				for (unsigned j = 0u; j < stateMachine->GetClipsSize(); ++j)
				{
					ImGui::PushID(j);
					char* clipName = new char[MAX_CLIP_NAME];
					strcpy(clipName, stateMachine->GetClipName(j).C_str());
					ImGui::InputText("Clip name", clipName, MAX_CLIP_NAME);
					stateMachine->SetClipName(j, HashString(clipName));

					bool clipLoop = stateMachine->GetClipLoop(j);
					if (ImGui::Checkbox("Loop", &clipLoop))
					{
						if (!clipLoop)
							stateMachine->SetClipLoop(j, false);
						else
							stateMachine->SetClipLoop(j, true);

						stateMachine->Save();
					}

					ImGui::SameLine();

					unsigned clipUID = stateMachine->GetClipResource(j);
					ResourceAnimation* animation = (ResourceAnimation*)App->resManager->Get(clipUID);
					if (ImGui::BeginCombo("", clipUID != 0u ? animation->name.c_str() : ""))
					{
						if (guiAnimations.empty())
						{
							guiAnimations = App->resManager->GetAnimationsNamesList(true);
						}
						for (int n = 0; n < guiAnimations.size(); n++)
						{
							bool is_selected = (clipUID != 0u ? animation->name == guiAnimations[n] : false);
							if (ImGui::Selectable(guiAnimations[n].c_str(), is_selected))
							{
								unsigned animUID = ((ResourceAnimation*)App->resManager->GetAnimationByName(guiAnimations[n].c_str()))->GetUID();
								stateMachine->SetClipResource(j, animUID);
								stateMachine->Save();
							}
							if (is_selected)
							{
								ImGui::SetItemDefaultFocus();
							}
						}
						ImGui::EndCombo();
					}
					else
					{
						guiAnimations.clear();
					}

					if (ImGui::Button("Remove Clip"))
					{
						clipIndexToRemove = j;
						clipRemove = true;
					}

					ImGui::Separator();

					ImGui::PopID();
				}
			}

			if (clipRemove)
			{
				stateMachine->RemoveClip(clipIndexToRemove);
				clipRemove = false;
			}
		}
	}
	ImGui::PopID();
}

void ComponentAnimation::ResetResource()
{
	anim->DeleteFromMemory();
	stateMachine->DeleteFromMemory();
}

void ComponentAnimation::SetAnimation(const char* animationFile)
{
	// Delete previous animation

	if (anim != nullptr)
		App->resManager->DeleteResource(anim->GetUID());

	if (animationFile != nullptr)
		anim = (ResourceAnimation*)App->resManager->GetAnimationByName(animationFile);

	return;
}

void ComponentAnimation::SetStateMachine(const char * stateMachineFile)
{
	// Delete previous stateMachine

	if (stateMachine != nullptr)
		App->resManager->DeleteResource(stateMachine->GetUID());

	if (stateMachineFile != nullptr)
		stateMachine = (ResourceStateMachine*)App->resManager->GetSMByName(stateMachineFile);
}

ComponentAnimation::EditorContext* ComponentAnimation::GetEditorContext()
{
	if (context == nullptr)
	{
		ax::NodeEditor::Config cfg;
		cfg.SettingsFile = "simple.json";
		context = ax::NodeEditor::CreateEditor(&cfg);
	}
	return context;
}

void ComponentAnimation::Update()
{
	PROFILE;
	if (anim != nullptr)
	{
		if (!channelsSetted)
		{
			SetIndexChannels(gameobject);
			channelsSetted = true;
		}

		if (App->time->gameState == GameState::RUN)
		{
			controller->Update(App->time->gameDeltaTime);

			if (gameobject != nullptr)
			{
				UpdateGO(gameobject);
			}
		}
		else if (isPlaying)
		{
			controller->Update(App->time->realDeltaTime);

			if (gameobject != nullptr)
			{
				UpdateGO(gameobject);
			}
		}
	}
}

void ComponentAnimation::UpdateGO(GameObject* go)
{
	PROFILE;
	float3 position;
	Quat rotation;

	if (controller->GetTransform(go->animationIndexChannel, position, rotation))
	{
		go->transform->SetPosition(position);
		go->transform->SetRotation(rotation);
	}

	gameobject->movedFlag = true;

	for (std::list<GameObject*>::iterator it = go->children.begin(); it != go->children.end(); ++it)
	{
		UpdateGO(*it);
	}
}

void ComponentAnimation::PlayAnimation(unsigned blend)
{
	controller->Play(anim, true, blend);
}

Component* ComponentAnimation::Clone() const
{
	return new ComponentAnimation(*this);
}


ComponentAnimation::ComponentAnimation(const ComponentAnimation& component) : Component(component)
{
	anim = (ResourceAnimation*)App->resManager->Get(component.anim->GetUID());
}


bool ComponentAnimation::CleanUp()
{
	if (anim != nullptr)
	{
		App->resManager->DeleteResource(anim->GetUID());
	}
	return true;
}

void ComponentAnimation::Save(JSON_value* value) const
{
	Component::Save(value);
	value->AddUint("animUID", (anim != nullptr) ? anim->GetUID() : 0u);
}

void ComponentAnimation::Load(JSON_value* value)
{
	Component::Load(value);
	unsigned uid = value->GetUint("animUID");

	anim = (ResourceAnimation*)App->resManager->Get(uid);
}

void ComponentAnimation::SetIndexChannels(GameObject* GO)
{
	GO->animationIndexChannel = anim->GetIndexChannel(GO->name.c_str());

	for (const auto& child : GO->children)
	{
		SetIndexChannels(child);
	}

}