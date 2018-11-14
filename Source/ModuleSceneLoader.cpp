#include "ModuleSceneLoader.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleCamera.h"
#include "ModuleTextures.h"
#include "ModuleProgram.h"
#include "ModuleScene.h"
#include "imgui.h"
#include "assimp/cimport.h"
#include "assimp/postprocess.h"
#include "GL/glew.h"
#include "GameObject.h"
#include "Component.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"

#define CHECKERS "checkersTexture.jpg"
#define BAKERHOUSE "Models\\Baker\\BakerHouse.fbx"

void AddLog(const char* str, char* userData) 
{
	std::string info(str);
	info.pop_back();

	LOG("%s", info.c_str());
}

ModuleSceneLoader::ModuleSceneLoader()
{
}


ModuleSceneLoader::~ModuleSceneLoader()
{
	//if (checkersTexture.id != 0)
	//{
	//	glDeleteTextures(1, (GLuint*)&checkersTexture.id);
	//}
}


bool ModuleSceneLoader::Init()
{
	struct aiLogStream streamLog;
	streamLog = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, NULL);
	streamLog.callback = AddLog;
	aiAttachLogStream(&streamLog);

	LoadFile(BAKERHOUSE); 
	return true;
}


void ModuleSceneLoader::LoadFile(const char *path)
{
	assert(path != nullptr);
	//If we already have models loaded, we erase them 
	//Only one model at a time for the moment
	const aiScene* scene = aiImportFile(path, aiProcess_Triangulate);
	if (scene == NULL)
	{
		LOG("ERROR importing file:%s \n", aiGetErrorString());
	}
	else
	{
		LoadScene(scene);
	}
	//DeleteModels();
	//models.emplace_back(path);
	//App->camera->Center();
	//Remove everything abov + if file is fbx then go to fbx loader
	//then in fbx loader, load scene and for each node load gameobject calling
	//createGameObject scene method? vs load file with multiple gameobjects in scene
}

void ModuleSceneLoader::LoadScene(const aiScene * scene)
{
	assert(scene != nullptr);
	ProcessNode(scene->mRootNode, scene, aiMatrix4x4());
	//for (unsigned i = 0; i < scene->mNumMaterials; ++i)
	//{
	//	GenerateMaterialData(scene->mMaterials[i]);
	//}
	//GetBoundingBox();
	aiReleaseImport(scene);
}

void ModuleSceneLoader::ProcessNode(const aiNode * node, const aiScene * scene, const aiMatrix4x4 & parentTransform)
{
	assert(node != nullptr); assert(scene != nullptr);
	aiMatrix4x4 transform = node->mTransformation*parentTransform;
	GameObject * gameobject = App->scene->CreateGameObject(transform);
	
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		ComponentMesh* mesh = (ComponentMesh*) gameobject->CreateComponent(ComponentType::Mesh);
		mesh->SetMesh(scene->mMeshes[node->mMeshes[i]]);

		ComponentMaterial* material = (ComponentMaterial*)gameobject->CreateComponent(ComponentType::Material);
		material->SetMaterial(scene->mMaterials[mesh->GetMaterialIndex()]);
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene, transform);
	}
}

void ModuleSceneLoader::DrawModels()
{
	//for (auto const& model : models)
	//{
	//	model.Draw();
	//}
}

void ModuleSceneLoader::DrawModelProperties()
{
	//if (models.size() == 0)
	//{
	//	ImGui::Text("No model loaded");
	//}

	//for (auto &model : models)
	//{
	//	ImGui::Text("Model:");
	//	ImGui::Separator();
	//	model.DrawProperties();
	//}
}

void ModuleSceneLoader::DrawGUI()
{
	//if (ImGui::Checkbox("Use Checkers Texture", &checkers))
	//{
	//	if (checkers && checkersTexture.id == 0)
	//	{
	//		checkersTexture = App->textures->Load(CHECKERS);
	//	}
	//}
}

void ModuleSceneLoader::ApplyTexture(Texture texture)
{
	//for (auto& model : models)
	//{
	//	model.UpdateTexture(texture);
	//}
}

void ModuleSceneLoader::DeleteModels()
{
	//models.clear();
}



bool ModuleSceneLoader::CleanUp()
{
	return true;
}
