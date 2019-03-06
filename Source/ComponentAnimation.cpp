#include "Application.h"

#include "ModuleResourceManager.h"
#include "ModuleFileSystem.h"

#include "Animation.h"
#include "ComponentAnimation.h"

#include "Imgui/include/imgui.h"
#include "JSON.h"

void ComponentAnimation::DrawProperties()
{
	//We need to have here:

	//Name of the animation
	ImGui::Text(anim->animationName.c_str());

	//Duration of the animation
	ImGui::Text("%f seconds", anim->duration);

	//Number of frames the animation has
	ImGui::Text("%f frames", anim->numberFrames);

	//Switch between animation frames
	ImGui::Text("Current Frame: %s", anim->animationName.c_str());


	//this is wrong, we dont want to iterate channels but frames

	//if (ImGui::InputInt("Frame #", &anim->currentFrame))
	//{
	//	if (anim->currentFrame < 0)
	//	{
	//		anim->currentFrame = anim->numberFrames - 1;
	//	}
	//	else if(anim->currentFrame >= anim->currentFrame)
	//	{
	//		anim->currentFrame = 0;
	//	}
	//	anim->currentChannel = anim->channels[anim->currentFrame];
	//}
	//Play
	ImGui::ArrowButton("Play", ImGuiDir_Right);
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

	//Does this work? not sure

	App->resManager->DeleteAnim(anim->UID);
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

