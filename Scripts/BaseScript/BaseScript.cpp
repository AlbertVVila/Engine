#include "BaseScript.h"

#include "Application.h"
#include "GameObject.h"
#include "imgui.h"

void Script::SetGameObject(GameObject* go)
{
	gameObject = go;
}

void Script::SetApp(Application* app)
{
	this->App = app;
}

void Script::Expose(ImGuiContext* context)
{
	ImGui::SetCurrentContext(context);
}
