#include "Application.h"

#include "ModuleResourceManager.h"
#include "ModuleFileSystem.h"
#include "ModuleTime.h"

#include "GameObject.h"
#include "Animation.h"
#include "AnimationController.h"
#include "ComponentAnimation.h"
#include "ComponentTransform.h"

#include "Imgui/include/imgui.h"
#include "JSON.h"
#include "Math/Quat.h"
#include "Math/float3.h"
#include "Brofiler.h"
#define MAX_NAME 64

ComponentAnimation::ComponentAnimation() : Component(nullptr, ComponentType::Animation)
{
	controller = new AnimationController();
	PlayAnimation(100u);
}


ComponentAnimation::~ComponentAnimation()
{
	anim = nullptr;
}


void ComponentAnimation::DrawProperties()
{
	if (ImGui::CollapsingHeader("Animation", ImGuiTreeNodeFlags_DefaultOpen))
	{
		char *anim_name = new char[MAX_NAME];
		strcpy(anim_name, anim->animationName.c_str());
		//Name of the animation
		ImGui::Text("Name");
		ImGui::InputText("##label", anim_name, MAX_NAME);
		anim->animationName = anim_name;

		//Number of frames the animation has
		ImGui::Text("%i frames,", anim->numberFrames);
		ImGui::SameLine();
		//Duration of the animation
		ImGui::Text("%i seconds", anim->durationInSeconds);
		ImGui::Spacing();
		//Play
		ImGui::PushItemWidth(50);
		ImGui::DragFloat("Animation Speed", &controller->current->speed, 0.01f, -2.0f, 2.0f);
		ImGui::PopItemWidth();

		// Loop
		ImGui::Checkbox("Loop", &controller->current->loop);

		ImGui::Separator();
	}
}

void ComponentAnimation::Update(float dt)
{
	PROFILE;

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

ComponentAnimation::ComponentAnimation(GameObject * gameobject) : Component(gameobject, ComponentType::Animation)
{
	anim = new Animation();
	controller = new AnimationController();
	PlayAnimation(100u);
}

ComponentAnimation::ComponentAnimation(const ComponentAnimation& component) : Component(component)
{
	anim = component.anim;

	App->resManager->AddAnim(anim);
}


bool ComponentAnimation::CleanUp()
{
	if (anim != nullptr)
	{
		App->resManager->DeleteAnim(anim->UID);
	}
	return true;
}

void ComponentAnimation::Save(JSON_value* value) const
{
	Component::Save(value);
	value->AddUint("animUID", anim->UID);
}

void ComponentAnimation::Load(JSON_value* value)
{
	Component::Load(value);
	unsigned uid = value->GetUint("animUID");

	Animation* a = App->resManager->GetAnim(uid);

	if (a != nullptr)
	{
		anim = a;

	}
	else
	{
		char* data = nullptr;
		App->fsystem->Load((ANIMATIONS + std::to_string(uid) + ANIMATIONEXTENSION).c_str(), &data);
		anim->Load(data, uid);
		controller->ResetClipping();
		App->resManager->AddAnim(anim);
	}
}

void ComponentAnimation::SetIndexChannels(GameObject* GO)
{
	GO->animationIndexChannel = anim->GetIndexChannel(GO->name.c_str());

	for (const auto& child : GO->children)
	{
		SetIndexChannels(child);
	}

}