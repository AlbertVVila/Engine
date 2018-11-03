#include "ModuleModel.h"
#include "GL/glew.h"
#include "assert.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleTextures.h"
#include "ModuleProgram.h"

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
	for (auto const& it : models)
	{
		it.Draw();
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
