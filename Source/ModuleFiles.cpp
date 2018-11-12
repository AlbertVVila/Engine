#include "ModuleFiles.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleCamera.h"
#include "ModuleTextures.h"
#include "ModuleProgram.h"
#include "ModuleScene.h"
#include "imgui.h"
#include "assimp/cimport.h"
#include "GL/glew.h"

#define CHECKERS "checkersTexture.jpg"
#define BAKERHOUSE "Models\\Baker\\BakerHouse.fbx"

void AddLog(const char* str, char* userData) 
{
	std::string info(str);
	info.pop_back();

	LOG("%s", info.c_str());
}

ModuleFiles::ModuleFiles()
{
}


ModuleFiles::~ModuleFiles()
{
	if (checkersTexture.id != 0)
	{
		glDeleteTextures(1, (GLuint*)&checkersTexture.id);
	}
}


bool ModuleFiles::Init()
{
	struct aiLogStream streamLog;
	streamLog = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, NULL);
	streamLog.callback = AddLog;
	aiAttachLogStream(&streamLog);

	LoadFile(BAKERHOUSE); //moure al start
	return true;
}


void ModuleFiles::LoadFile(const char *path)
{
	assert(path != NULL);
	//If we already have models loaded, we erase them 
	//Only one model at a time for the moment
	DeleteModels();
	models.emplace_back(path);
	App->camera->Center();
	//Remove everything abov + if file is fbx then go to fbx loader
	//then in fbx loader, load scene and for each node load gameobject calling
	//createGameObject scene method? vs load file with multiple gameobjects in scene
}

void ModuleFiles::DrawModels()
{
	for (auto const& model : models)
	{
		model.Draw();
	}
}

void ModuleFiles::DrawModelProperties()
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

void ModuleFiles::DrawGUI()
{
	if (ImGui::Checkbox("Use Checkers Texture", &checkers))
	{
		if (checkers && checkersTexture.id == 0)
		{
			checkersTexture = App->textures->Load(CHECKERS);
		}
	}
}

void ModuleFiles::ApplyTexture(Texture texture)
{
	for (auto& model : models)
	{
		model.UpdateTexture(texture);
	}
}

void ModuleFiles::DeleteModels()
{
	models.clear();
}



bool ModuleFiles::CleanUp()
{
	return true;
}
