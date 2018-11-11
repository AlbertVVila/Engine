#include "ModuleModel.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleCamera.h"
#include "ModuleTextures.h"
#include "ModuleProgram.h"
#include "imgui.h"

#define CHECKERS "checkersTexture.jpg"
#define BAKERHOUSE "Models\\Baker\\BakerHouse.fbx"

void AddLog(const char* str, char* userData) 
{
	std::string info(str);
	info.pop_back();

	LOG("%s", info.c_str());
}

ModuleModel::ModuleModel()
{
}


ModuleModel::~ModuleModel()
{
}


bool ModuleModel::Init()
{
	struct aiLogStream streamLog;
	streamLog = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, NULL);
	streamLog.callback = AddLog;
	aiAttachLogStream(&streamLog);

	Load(BAKERHOUSE); //moure al start
	return true;
}


unsigned int ModuleModel::Load(const char *path)
{
	assert(path != NULL);
	//If we already have models loaded, we erase them 
	//Only one model at a time for the moment
	DeleteModels();
	models.emplace_back(path);
	App->camera->Center();
	return 1;
}

void ModuleModel::DrawModels()
{
	for (auto const& model : models)
	{
		model.Draw();
	}
}

void ModuleModel::DrawModelProperties()
{
	if (models.size() == 0)
	{
		ImGui::Text("No model loaded");
	}

	for (auto &model : models)
	{
		ImGui::Text("Model:");
		ImGui::Separator();
		model.DrawProperties();
	}
}

void ModuleModel::DrawGUI()
{
	if (ImGui::Checkbox("Use Checkers Texture", &checkers))
	{
		if (checkers && checkersTexture.id == 0)
		{
			checkersTexture = App->textures->Load(CHECKERS);
		}
	}
}

void ModuleModel::ApplyTexture(Texture texture)
{
	for (auto& model : models)
	{
		model.UpdateTexture(texture);
	}
}

void ModuleModel::DeleteModels()
{
	models.clear();
}



bool ModuleModel::CleanUp()
{
	return true;
}
