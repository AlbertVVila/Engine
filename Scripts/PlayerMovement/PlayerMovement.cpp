#include "PlayerMovement.h"
//#include "JSON.h"
#include <assert.h>
#include "imgui.h"
#include <iostream>

PlayerMovement_API PlayerMovement* CreateScript()
{
	PlayerMovement* instance = new PlayerMovement;
	return instance;
}

void PlayerMovement::Expose(ImGuiContext * context)
{
	ImGui::SetCurrentContext(context);
	ImGui::InputFloat("speed", &speed);
}

void PlayerMovement::Update()
{
	std::cout << "speed : " << speed << std::endl;
}

//void PlayerMovement::Serialize(JSON * json) const //TODO: Create template base method
//{
//	assert(json != nullptr); //JSON is null
//	JSON_value* variables = json->CreateValue();
//	variables->AddFloat("speed", speed);
//	json->AddValue("variables", *variables);
//}
//
//void PlayerMovement::DeSerialize(JSON * json)
//{
//	JSON_value* variables = json->GetValue("variables");
//	speed = variables->GetFloat("speed");
//}
