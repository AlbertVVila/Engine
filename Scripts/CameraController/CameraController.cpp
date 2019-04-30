#include "CameraController.h"
#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "imgui.h"

CameraController_API Script* CreateScript()
{
	CameraController* instance = new CameraController;
	return instance;
}

void CameraController::Expose(ImGuiContext * context)
{
	ImGui::InputFloat("camera Height", &height);
}

void CameraController::Start()
{
	player = App->scene->FindGameObjectByName(App->scene->root, "Player");
	assert(player != nullptr);
}

void CameraController::Update()
{
	math::float3 newPosition = player->transform->GetPosition();
	newPosition.y += height;
	gameObject->transform->SetPosition(newPosition);
}
