#include "ModuleModel.h"
#include "GL/glew.h"
#include "assert.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleTextures.h"
#include "ModuleProgram.h"
#include "imgui.h"

ModuleModel::ModuleModel()
{
}


ModuleModel::~ModuleModel()
{
}

bool ModuleModel::Init()
{
	return true;
}


unsigned int ModuleModel::Load(const char *path)
{
	//If we already have models loaded, we erase them 
	//Only one model at a time for the moment
	DeleteModels();
	models.emplace_back(path);

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
		ImGui::Text("Properties:");
		ImGui::Separator();
		model.DrawProperties();
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
