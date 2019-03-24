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


ComponentAnimation::ComponentAnimation() : Component(nullptr, ComponentType::Animation)
{
	controller = new AnimationController();
}


ComponentAnimation::~ComponentAnimation()
{
	anim = nullptr;
}


void ComponentAnimation::DrawProperties()
{
	if (ImGui::CollapsingHeader("Animation", ImGuiTreeNodeFlags_DefaultOpen))
	{

		//Name of the animation
		ImGui::Text(anim->animationName.c_str());

		//Number of frames the animation has
		ImGui::Text("%i frames", anim->numberFrames);

		//Duration of the animation
		ImGui::Text("%i seconds", anim->durationInSeconds);

		//Play
		ImGui::DragFloat("Animation Speed", &controller->current->speed, 0.01f, -2.0f, 2.0f);
	}
}

void ComponentAnimation::Update(float dt)
{
	if (App->time->gameState == GameState::RUN)
	{
		controller->Update(App->time->gameDeltaTime);

		if (gameobject != nullptr)
		{
			UpdateGO(gameobject);
		}
	}
}

void ComponentAnimation::UpdateGO(GameObject* go)
{
	float3 position;
	Quat rotation;

	if (controller->GetTransform(go->name.c_str(), position, rotation))
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
		App->resManager->AddAnim(anim);
	}
}