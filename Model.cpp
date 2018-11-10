#include "Model.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleProgram.h"
#include "imgui.h"

Model::Model(const char * file)
{
	std::string s(file);
	std::size_t found = s.find_last_of("/\\");
	s = s.substr(0, found+1);
	this->path = s.c_str();
	LoadModel(file);
}

Model::~Model()
{
}

void Model::LoadModel(const char * path)
{
	assert(path != NULL);
	const aiScene* scene = aiImportFile(path, aiProcess_Triangulate);
	if (scene == NULL)
	{
		LOG("ERROR importing file:%s \n", aiGetErrorString());
	}

	ProcessNode(scene->mRootNode, scene);

	for (unsigned i = 0; i < scene->mNumMaterials; ++i)
	{
		GenerateMaterialData(scene->mMaterials[i]);
	}
	GetBoundingBox();
}
	

void Model::GenerateMaterialData(aiMaterial * material)
{
	aiTextureMapping mapping = aiTextureMapping_UV;
	aiString file;
	material->GetTexture(aiTextureType_DIFFUSE, 0, &file, &mapping, 0);

	std::string texturePath(path);
	texturePath += file.C_Str();
	Texture texture = App->textures->Load(texturePath.c_str());
	textures.push_back(texture);
}

void Model::Draw() const
{
	for (auto& mesh : meshes)
	{
		mesh.Draw(App->program->textureProgram, textures);
	}
}

void Model::DrawProperties()
{
	if (ImGui::CollapsingHeader("Geometry"))
	{
		float3 size = BoundingBox.Size();
		ImGui::Text("Size:");
		ImGui::Text("X: %.2f", size.x); ImGui::SameLine();
		ImGui::Text("Y: %.2f", size.y); ImGui::SameLine();
		ImGui::Text("Z: %.2f", size.z);
		ImGui::Separator();

		for (auto& mesh : meshes)
		{
			ImGui::Text("Mesh:");
			ImGui::Text("Number of Triangles: %d", mesh.numIndices / 3);
			ImGui::Separator();

			if (ImGui::CollapsingHeader("Transformation"))
			{

				ImGui::Text("Position:");  ImGui::SameLine();
				ImGui::Text("X: %.2f", mesh.localPosition.x); ImGui::SameLine();
				ImGui::Text("Y: %.2f", mesh.localPosition.y); ImGui::SameLine();
				ImGui::Text("Z: %.2f", mesh.localPosition.z);


				ImGui::Text("Rotation:");  ImGui::SameLine();
				float3 rotation = mesh.localRotation.ToEulerXYZ();
				ImGui::Text("X: %.2f", math::RadToDeg(rotation.x)); ImGui::SameLine();
				ImGui::Text("Y: %.2f", math::RadToDeg(rotation.y)); ImGui::SameLine();
				ImGui::Text("Z: %.2f", math::RadToDeg(rotation.z));

				ImGui::Text("Scale:");  ImGui::SameLine();
				ImGui::Text("X: %.2f", mesh.localScale.x); ImGui::SameLine();
				ImGui::Text("Y: %.2f", mesh.localScale.y); ImGui::SameLine();
				ImGui::Text("Z: %.2f", mesh.localScale.z);
			}
		}
	}
	if (ImGui::CollapsingHeader("Textures"))
	{
		for (auto &texture : textures)
		{
			ImGui::Text("Size:  Width: %d | Height: %d",texture.width,texture.height);
			float size = ImGui::GetWindowWidth();
			ImGui::Image((ImTextureID)texture.id, { size,size });
		}
	}

}

void Model::UpdateTexture(Texture texture)
{
	for (auto &Oldtexture : textures)
	{
		Oldtexture = texture;
	}
}

void Model::ProcessNode(aiNode *node, const aiScene *scene)
{
	aiMatrix4x4 transform = node->mTransformation;
	//aiVector3D scale, translation;
	//aiQuaternion rotation;
	//transformation.Decompose(scale, rotation, translation);
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.emplace_back(mesh, transform);
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene);
	}
}

void Model::GetBoundingBox()
{
	if (meshes.size() == 0 || meshes.front().vertices.size() == 0) return;

	float3 min, max;
	min = max = meshes.front().vertices[0];
	for (auto &mesh : meshes)
	{
		for (auto &vertice : mesh.vertices)
		{
			min.x = MIN(min.x, vertice.x);
			min.y = MIN(min.y, vertice.y);
			min.z = MIN(min.z, vertice.z);

			max.x = MAX(max.x, vertice.x);
			max.y = MAX(max.y, vertice.y);
			max.z = MAX(max.z, vertice.z);

		}
	}
	BoundingBox.minPoint = min;
	BoundingBox.maxPoint = max;
}