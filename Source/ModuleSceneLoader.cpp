#include "ModuleSceneLoader.h"
#include "ModuleRender.h"
#include "ModuleCamera.h"
#include "ModuleTextures.h"
#include "ModuleProgram.h"
#include "ModuleScene.h"
#include "ModuleFileSystem.h"
#include "GL/glew.h"
#include "Math/float4.h"


#include "Application.h"
#include "GameObject.h"
#include "Component.h"


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
	//struct aiLogStream streamLog;
	//streamLog = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, NULL);
	//streamLog.callback = AddLog;
	//aiAttachLogStream(&streamLog);
 
	return true;
}

bool ModuleSceneLoader::Start()
{
	//LoadFile(BAKERHOUSE);
	//FileImporter sceneImporter;
	//sceneImporter.ImportImage("Models\\Baker\\Baker_house.png");
	//std::string outputfile = "BakerHouse.dat";
	//sceneImporter.ImportFBX("BakerHouse.fbx",BAKERHOUSE, outputfile);
	//char* bakerhouse = nullptr;
	//sceneImporter.LoadData(outputfile, bakerhouse);
	//LoadScene(bakerhouse);
	//CreateSphere("sphere0", float3(0.0f, 0.0f, 0.0f), Quat::identity, 1.0f, 20, 20, float4(1.f, 0.0f, 0.f, 1.0f));
	//App->scene->SaveScene();
	//App->scene->LoadScene("scene.json");
	return true;
}

void ModuleSceneLoader::LoadScene(char *data)//TODO: make data constant and save size of components
{
	//assert(data != nullptr);
	//if (data == nullptr)
	//{
	//	LOG("No content to load!!");
	//	return;
	//}
	//char* cursor = data;
	//ProcessGameObject(cursor, float4x4::identity, App->scene->root);
	//delete[] data;
}

//GameObject* ModuleSceneLoader::ProcessGameObject(char * &cursor, float4x4 parentTransform, GameObject *parent) 
//{
//	assert(cursor != nullptr);
//
//	int id = *(int*)cursor;
//	cursor += sizeof(int);
//
//	int parent_id = *(int*)cursor;
//	cursor += sizeof(int);
//
//	float4x4 mytransform;
//	mytransform.Set((float*)cursor);
//	cursor += sizeof(float) * 16;
//
//	float4x4 transform = parentTransform * mytransform;
//
//	std::string name(cursor);
//	cursor += (name.length()+1)*sizeof(char);
//	GameObject * gameobject = App->scene->CreateGameObject(transform, "", name.c_str(), parent); //TODO: refactor filepath variable
//
//	unsigned int numMeshes = *(int*)cursor;
//	cursor += sizeof(int);
//
//	for (unsigned int i = 0; i < numMeshes; i++)
//	{
//		ComponentMesh* mesh = (ComponentMesh*) gameobject->CreateComponent(ComponentType::Mesh);
//		mesh->SetMesh(cursor); //SetMesh moves the cursor at the end of the mesh
//		
//		std::string materialFile(cursor);
//		cursor += sizeof(char)*(materialFile.length() + 1);
//		ComponentMaterial* material = (ComponentMaterial*)gameobject->CreateComponent(ComponentType::Material);
//		//material->SetMaterial(scene->mMaterials[mesh->GetMaterialIndex()]); //TODO: Read materials apart 
//	}
//
//	unsigned int numChildren = *(int*)cursor;
//	cursor += sizeof(int);
//
//	for (unsigned int i = 0; i < numChildren; i++)
//	{
//		GameObject * child = ProcessGameObject(cursor, transform, gameobject);
//	}
//
//	return gameobject;
//}

//
//void ModuleSceneLoader::LoadFile(const char *path)
//{
//	assert(path != nullptr);
//	//If we already have models loaded, we erase them 
//	//Only one model at a time for the moment
//	const aiScene* scene = aiImportFile(path, aiProcess_Triangulate);
//	if (scene == NULL)
//	{
//		LOG("ERROR importing file:%s \n", aiGetErrorString());
//	}
//	else
//	{
//		filepath = path;
//		LoadScene(scene);
//	}
//}

//void ModuleSceneLoader::LoadScene(const aiScene * scene)
//{
//	assert(scene != nullptr);
//	ProcessNode(scene->mRootNode, scene, aiMatrix4x4(), App->scene->root);
//	//for (unsigned i = 0; i < scene->mNumMaterials; ++i)
//	//{
//	//	GenerateMaterialData(scene->mMaterials[i]);
//	//}
//	aiReleaseImport(scene);
//}
//
//GameObject* ModuleSceneLoader::ProcessNode(const aiNode * node, const aiScene * scene, const aiMatrix4x4 & parentTransform, GameObject* parent)
//{
//	assert(node != nullptr); assert(scene != nullptr);
//	aiMatrix4x4 transform = parentTransform * node->mTransformation;
//	GameObject * gameobject = App->scene->CreateGameObject(transform, filepath, node->mName.C_Str(), parent);
//
//	for (unsigned int i = 0; i < node->mNumMeshes; i++)
//	{
//		ComponentMesh* mesh = (ComponentMesh*) gameobject->CreateComponent(ComponentType::Mesh);
//		mesh->SetMesh(scene->mMeshes[node->mMeshes[i]]);
//
//		ComponentMaterial* material = (ComponentMaterial*)gameobject->CreateComponent(ComponentType::Material);
//		material->SetMaterial(scene->mMaterials[mesh->GetMaterialIndex()]); //TODO: Read materials first
//	}
//	for (unsigned int i = 0; i < node->mNumChildren; i++)
//	{
//		GameObject * child = ProcessNode(node->mChildren[i], scene, transform, gameobject);
//	}
//	return gameobject;
//}


bool ModuleSceneLoader::CleanUp()
{
	return true;
}
