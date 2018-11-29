#include "MeshImporter.h"
#include <assert.h>
#include "assimp/cimport.h"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include <assimp/mesh.h>

MeshImporter::MeshImporter()
{
}


MeshImporter::~MeshImporter()
{
}

bool MeshImporter::Import(const char * file, const char * path, std::string & output_file)
{
	assert(path != nullptr);
	//If we already have models loaded, we erase them 
	//Only one model at a time for the moment
	const aiScene* scene = aiImportFile(path, aiProcess_Triangulate);
	if (scene != nullptr)
	{
		SaveScene(scene);
	}
	return true;
}


void MeshImporter::SaveScene(const aiScene * scene)
{
	assert(scene != nullptr);
	SaveNode(*scene->mRootNode, *scene, aiMatrix4x4());
	aiReleaseImport(scene);
}

void MeshImporter::SaveNode(const aiNode& node, const aiScene & scene, const aiMatrix4x4 & parentTransform)
{
	aiMatrix4x4 transform = parentTransform * node.mTransformation;
	//GameObject * gameobject = App->scene->CreateGameObject(transform, filepath, node->mName.C_Str(), parent);

	for (unsigned int i = 0; i < node.mNumMeshes; i++)
	{
		//TODO: SAVE MESH OF CURRENT NODE
		SaveMesh(*scene.mMeshes[node.mMeshes[i]]); //Add id
		//ComponentMesh* mesh = (ComponentMesh*)gameobject->CreateComponent(ComponentType::Mesh);
		//mesh->SetMesh(scene->mMeshes[node->mMeshes[i]]);

		//ComponentMaterial* material = (ComponentMaterial*)gameobject->CreateComponent(ComponentType::Material);
		//material->SetMaterial(scene->mMaterials[mesh->GetMaterialIndex()]); //TODO: Read materials first
	}
	for (unsigned int i = 0; i < node.mNumChildren; i++)
	{
		//TODO: SAVE MESH OF CHILD NODES
		//ProcessNode(node->mChildren[i], scene, transform, gameobject);
	}
}

void MeshImporter::SaveMesh(const aiMesh& mesh)
{
	unsigned int ranges[2] = { mesh.mNumFaces * 3, 	mesh.mNumVertices};
}
