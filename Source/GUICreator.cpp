#include "Application.h"
#include "ModuleScene.h"

#include "GameObject.h"
#include "GUICreator.h"
#include "imgui.h"

GUICreator::GUICreator()
{
}


GUICreator::~GUICreator()
{
}

void GUICreator::CreateElements(GameObject* go)
{
	assert(go != nullptr);
	if (go == nullptr) return;

	if (ImGui::BeginMenu("Create"))
	{
		if (ImGui::Selectable("Empty GameObject"))
		{
			App->scene->CreateGameObject("Empty", go);
		}
		if (ImGui::BeginMenu("Light"))
		{
			const char* lights[LIGHTTYPES] = { "Directional", "Point", "Spot" };
			for (unsigned i = 0; i < LIGHTTYPES; ++i)
			{
				if (ImGui::MenuItem(lights[i]))
				{
					GameObject *light = App->scene->CreateGameObject(lights[i], App->scene->root);
					light->CreateComponent(ComponentType::Transform);
					ComponentLight* lighttype = (ComponentLight *)light->CreateComponent(ComponentType::Light);
					lighttype->lightType = (LightType)i;
				}
			}
			ImGui::EndMenu();
		}
		if (ImGui::Selectable("Sphere"))
		{
			App->scene->CreateSphere("sphere", go);
		}
		if (ImGui::Selectable("Cube"))
		{
			App->scene->CreateCube("cube", go);
		}
		ImGui::EndMenu();
	}
}
