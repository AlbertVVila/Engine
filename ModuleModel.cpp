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
	Model model(path);
	models.push_back(model);

	return 1;
}

void ModuleModel::DrawModels()
{
	for (auto const& it : models)
	{
		it.Draw();
	}
}



bool ModuleModel::CleanUp()
{
	return true;
}
