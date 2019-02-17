#include "BaseScript.h"

#include "GameObject.h"
#include "imgui.h"

void Script::SetGameObject(GameObject * go)
{
	gameObject = go;
}

void Script::Expose(ImGuiContext *context)
{
	ImGui::SetCurrentContext(context);
}

void Script::Start()
{
}

void Script::Update()
{
}

int Script::GetWord()
{
	return 5;
}