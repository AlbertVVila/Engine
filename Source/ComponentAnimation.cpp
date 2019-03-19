#include "Application.h"

#include "ModuleResourceManager.h"
#include "ModuleFileSystem.h"

#include "GameObject.h"
#include "Animation.h"
#include "AnimationController.h"
#include "ComponentAnimation.h"
#include "ComponentTransform.h"

#include "Imgui/include/imgui.h"
#include "JSON.h"
#include "Math/Quat.h"
#include "Math/float3.h"

void ComponentAnimation::DrawProperties()
{
	//We need to have here:

	//Name of the animation
	ImGui::Text(anim->animationName.c_str());

	//Number of frames the animation has
	ImGui::Text("%f frames", anim->numberFrames);

	//Duration of the animation
	ImGui::Text("%i seconds", anim->durationInSeconds);

	//Switch between animation frames
	if (ImGui::InputInt("Frame #", &anim->currentSample))
	{
		if (anim->currentSample < 0)
		{
			anim->currentSample = anim->numberFrames - 1;
		}
		else if (anim->currentSample >= anim->numberFrames)
		{
			anim->currentSample = 0;
		}
	}

	//Play
	ImGui::ArrowButton("Play", ImGuiDir_Right);

}

void ComponentAnimation::Update()
{
	UpdateGO(gameobject);
}

void ComponentAnimation::UpdateGO(GameObject* go)
{
	/*float3 position;
	Quat rotation;

	if (controller->GetTransform(go->name.c_str(), position, rotation))
	{
		gameobject->transform->SetPosition(position);
		gameobject->transform->SetRotation(rotation);
	}

	for (std::list<GameObject*>::iterator it = go->children.begin(); it != go->children.end(); ++it)
	{
		UpdateGO(*it);
	}*/
}

Component* ComponentAnimation::Clone() const
{
	return new ComponentAnimation(*this);
}

ComponentAnimation::ComponentAnimation(GameObject * gameobject) : Component(gameobject, ComponentType::Animation)
{
	anim = new Animation();
}

ComponentAnimation::ComponentAnimation(const ComponentAnimation& component) : Component(component)
{
	anim = component.anim;

	App->resManager->AddAnim(anim);
}


ComponentAnimation::ComponentAnimation() : Component(nullptr, ComponentType::Animation)
{
	controller = new AnimationController();
}


ComponentAnimation::~ComponentAnimation()
{
	anim = nullptr;
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