#include "Application.h"
#include "ModuleScene.h"

#include "MaterialEditor.h"
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
			GameObject *newgo = App->scene->CreateGameObject("Empty", go);
			App->scene->Select(newgo);
		}
		if (ImGui::BeginMenu("Light"))
		{
			const char* lights[LIGHTTYPES] = { "Directional", "Point", "Spot" };
			for (unsigned i = 0; i < LIGHTTYPES; ++i)
			{
				if (ImGui::MenuItem(lights[i]))
				{
					GameObject *light = App->scene->CreateGameObject(lights[i], go);
					light->CreateComponent(ComponentType::Transform);
					ComponentLight* lighttype = (ComponentLight *)light->CreateComponent(ComponentType::Light);
					lighttype->lightType = (LightType)i;
					App->scene->Select(light);
				}
			}
			ImGui::EndMenu();
		}
		if (ImGui::Selectable("Camera"))
		{
			GameObject *cam = App->scene->CreateGameObject("Camera", go);
			cam->CreateComponent(ComponentType::Transform);
			cam->CreateComponent(ComponentType::Camera);
			App->scene->Select(cam);
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
