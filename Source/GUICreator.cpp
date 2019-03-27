#include "Application.h"
#include "ModuleScene.h"
#include "ModuleSpacePartitioning.h"

#include "MaterialEditor.h"
#include "GameObject.h"
#include "GUICreator.h"
#include "imgui.h"
#include "AABBTree.h"

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
					App->spacePartitioning->aabbTreeLighting.ReleaseNode(light->treeNode); //The aabbtree should be updated with the new light type
					App->spacePartitioning->aabbTreeLighting.InsertGO(light);
				}
			}
			ImGui::EndMenu();
		}
		
		if (ImGui::BeginMenu("UI"))
		{
			if (ImGui::Selectable("Text"))
			{				
				GameObject *newgo = new GameObject("Text", App->scene->GetNewUID());
				newgo->CreateComponent(ComponentType::Transform2D);
				newgo->CreateComponent(ComponentType::Text);
				App->scene->canvas->InsertChild(newgo);
				App->scene->Select(newgo);
			}

			if (ImGui::Selectable("Image"))
			{
				GameObject *newgo = new GameObject("Image", App->scene->GetNewUID());
				newgo->CreateComponent(ComponentType::Transform2D);
				newgo->CreateComponent(ComponentType::Image);
				App->scene->canvas->InsertChild(newgo);
				App->scene->Select(newgo);
			}

			if (ImGui::Selectable("Button"))
			{
				GameObject *newgo = new GameObject("Button", App->scene->GetNewUID());
				newgo->CreateComponent(ComponentType::Transform2D);
				newgo->CreateComponent(ComponentType::Button);
				App->scene->canvas->InsertChild(newgo);
				App->scene->Select(newgo);
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

		/*if (ImGui::BeginMenu("Navigation"))
		{
			if (ImGui::Selectable("Agent"))
			{
				GameObject *newgo = new GameObject("Text", App->scene->GetNewUID());
				newgo->CreateComponent(ComponentType::Transform2D);
				newgo->CreateComponent(ComponentType::Text);
				App->scene->canvas->InsertChild(newgo);
				App->scene->Select(newgo);
			}

			if (ImGui::Selectable("Obstacle"))
			{
				GameObject *newgo = new GameObject("Button", App->scene->GetNewUID());
				newgo->CreateComponent(ComponentType::Transform2D);
				newgo->CreateComponent(ComponentType::Button);
				App->scene->canvas->InsertChild(newgo);
				App->scene->Select(newgo);
			}
			ImGui::EndMenu();
		}*/

		ImGui::EndMenu();
	}
}
