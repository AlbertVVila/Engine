#include "Animation.h"

#include "ComponentAnimation.h"


#include "Imgui/imgui.h"
#include "JSON.h"

void ComponentAnimation::DrawProperties()
{
	//We need to have here:
	//Name of the animation
	ImGui::Text("");

	//Duration of the animation
	ImGui::Text("%f Frames", anim->duration);
	//Number of frames the animation has

	//Switch between animation frames
	/*ImGui::InputInt("Frames", )*/
	//Play
	ImGui::ArrowButton("Play", ImGuiDir_Right);



}

Component* ComponentAnimation::Clone() const
{
	return new ComponentAnimation(*this);
}

ComponentAnimation::ComponentAnimation(GameObject * gameobject) : Component(gameobject, ComponentType::Animation)
{

}

ComponentAnimation::ComponentAnimation(const ComponentAnimation& component) : Component(component)
{

}


ComponentAnimation::ComponentAnimation() : Component(nullptr, ComponentType::Animation)
{
}


ComponentAnimation::~ComponentAnimation()
{
}

void ComponentAnimation::Save(JSON_value * value) const
{
	Component::Save(value);
}

void ComponentAnimation::Load(const JSON_value & value)
{
	Component::Load(value);
}
