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

	ProcessNode(scene->mRootNode, scene, aiMatrix4x4());

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
	if (ImGui::CollapsingHeader("Model Geometry"))
	{
		float3 size = BoundingBox.Size();
		ImGui::InputFloat3("Size", (float*)&size, 2, ImGuiInputTextFlags_ReadOnly);

		ImGui::Separator();
		int numMesh = 0;
		ImGui::Text("Number of meshes: %d", meshes.size());
		for (auto& mesh : meshes)
		{
			ImGui::PushID("Mesh" + numMesh);
			if (ImGui::TreeNode("Mesh"))
			{
				ImGui::Text("Number of Triangles: %d", mesh.numIndices / 3);
				ImGui::Separator();

				if (ImGui::TreeNode("Transform"))
				{
					ImGui::InputFloat3("Position", (float*)&mesh.localPosition, 2, ImGuiInputTextFlags_ReadOnly);

					float3 rotation = mesh.localRotation.ToEulerXYZ();
					rotation.x = math::RadToDeg(rotation.x);
					rotation.y = math::RadToDeg(rotation.y);
					rotation.z = math::RadToDeg(rotation.z);

					ImGui::InputFloat3("Rotation", (float*)&rotation, 2, ImGuiInputTextFlags_ReadOnly);


					ImGui::InputFloat3("Scale", (float*)&mesh.localScale, 2, ImGuiInputTextFlags_ReadOnly);

					ImGui::TreePop();
				}

				ImGui::TreePop();
				ImGui::Separator();
			}
			ImGui::PopID();
			++numMesh;
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

void Model::ProcessNode(aiNode *node, const aiScene *scene, const aiMatrix4x4 parentTransform)
{
	aiMatrix4x4 transform = node->mTransformation*parentTransform;

	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.emplace_back(mesh, transform);
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene, transform);
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