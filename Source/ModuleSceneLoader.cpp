#include "ModuleSceneLoader.h"
#include "ModuleRender.h"
#include "ModuleCamera.h"
#include "ModuleTextures.h"
#include "ModuleProgram.h"
#include "ModuleScene.h"
#include "imgui.h"
#include "assimp/cimport.h"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "GL/glew.h"

#include "Application.h"
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
 
	return true;
}

bool ModuleSceneLoader::Start()
{
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
		filepath = path;
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
	ProcessNode(scene->mRootNode, scene, aiMatrix4x4(), App->scene->root);
	//for (unsigned i = 0; i < scene->mNumMaterials; ++i)
	//{
	//	GenerateMaterialData(scene->mMaterials[i]);
	//}
	aiReleaseImport(scene);
}

GameObject* ModuleSceneLoader::ProcessNode(const aiNode * node, const aiScene * scene, const aiMatrix4x4 & parentTransform, GameObject* parent)
{
	assert(node != nullptr); assert(scene != nullptr);
	aiMatrix4x4 transform = parentTransform * node->mTransformation;
	GameObject * gameobject = App->scene->CreateGameObject(transform, filepath, node->mName.C_Str(), parent);

	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		ComponentMesh* mesh = (ComponentMesh*) gameobject->CreateComponent(ComponentType::Mesh);
		mesh->SetMesh(scene->mMeshes[node->mMeshes[i]]);

		ComponentMaterial* material = (ComponentMaterial*)gameobject->CreateComponent(ComponentType::Material);
		material->SetMaterial(scene->mMaterials[mesh->GetMaterialIndex()]); //TODO: Read materials first
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		GameObject * child = ProcessNode(node->mChildren[i], scene, transform, gameobject);
	}
	return gameobject;
}


//void ModuleSceneLoader::ApplyTexture(Texture texture)
//{
	//for (auto& model : models)
	//{
	//	model.UpdateTexture(texture);
	//}
//}



bool ModuleSceneLoader::CleanUp()
{
	return true;
}
