//#include "Application.h"
//
//#include "ModuleTextures.h"
//#include "ModuleFileSystem.h"
//#include "ModuleResourceManager.h"
//
//#include "ComponentMesh.h"
//
//#include <assert.h>
//#include "GL/glew.h"
//#include "Imgui/imgui.h"
//#include "JSON.h"
//
//ComponentMesh::ComponentMesh(GameObject* gameobject) : Component(gameobject, ComponentType::Mesh)
//{
//	mesh = new Mesh();
//}
//
//ComponentMesh::ComponentMesh(const ComponentMesh & component) : Component(component) //TODO: SaveMesh (for sphere, cube, etc)
//{
//	mesh = component.mesh;
//	App->resManager->AddMesh(mesh);
//}
//
//ComponentMesh::~ComponentMesh()
//{
//	DeleteBuffers(); //TODO: clean up components or simply call destructor?
//}
//
//Component * ComponentMesh::Clone() const
//{
//	return new ComponentMesh(*this);
//}
//
//void ComponentMesh::Draw(unsigned int shaderProgram) const
//{
//	glBindVertexArray(mesh->VAO);
//	glDrawElements(GL_TRIANGLES, mesh->numIndices, GL_UNSIGNED_INT, 0);
//
//	// We disable VAO
//	glBindVertexArray(0);
//	// We disable Texture
//}
//
//void ComponentMesh::DrawProperties()
//{
//	ImGui::PushID(this);
//	if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen))
//	{
//		bool removed = Component::DrawComponentState();
//		if (removed)
//		{
//			ImGui::PopID();
//			return;
//		}
//		ImGui::Text("Num vertices : %d", mesh->vertices.size());
//		ImGui::Text("Num triangles : %d", mesh->numIndices / 3);
//		ImGui::Separator();
//	}
//	ImGui::PopID();
//}
//
//void ComponentMesh::SetMesh(char * &meshData, unsigned uid) //TODO: pass by reference or know size of mesh
//{
//	assert(meshData != nullptr);
//	DeleteBuffers();
//	char *data = meshData;
//
//	unsigned int numIndices = *(int*)meshData;
//	meshData += sizeof(int);
//
//	unsigned int numVertices = *(int*)meshData;
//	meshData += sizeof(int);
//
//	float* vertices = (float*)meshData;
//	meshData += sizeof(float) * 3 * numVertices;
//	
//	bool hasNormals = (bool*)meshData;
//	meshData += sizeof(bool);
//
//	float* normals = nullptr;
//	if (hasNormals)
//	{
//		normals = (float*)meshData;
//		meshData += sizeof(float) * 3 * numVertices;
//	}
//
//	bool hasTexCoords = (bool*)meshData;
//	meshData += sizeof(bool);
//
//	float* texCoords = nullptr;
//	if (hasTexCoords)
//	{
//		texCoords = (float*)meshData;
//		meshData += sizeof(float) * 2 * numVertices;
//	}
//
//	int* indices = (int*)meshData;
//	meshData += sizeof(int) * numIndices;
//	// VAO Creation
//	glGenVertexArrays(1, &mesh->VAO);
//	glBindVertexArray(mesh->VAO);
//
//	// Buffer Creation with vertex data
//	glGenBuffers(1, &mesh->VBO);
//	glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
//	unsigned bufferSize = sizeof(GLfloat)*numVertices * 3;
//	if (hasNormals) bufferSize += sizeof(GLfloat)*numVertices * 3;
//	if (hasTexCoords) bufferSize+= sizeof(GLfloat)*numVertices * 2;
//	glBufferData(GL_ARRAY_BUFFER, bufferSize, NULL, GL_STATIC_DRAW);
//
//	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * 3 * numVertices, vertices);
//
//	if (hasNormals)
//	{
//		glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * numVertices, sizeof(GLfloat) * 3 * numVertices, normals);
//	}
//	unsigned offsetTexCoords = 0;
//	if (hasTexCoords)
//	{
//		offsetTexCoords = hasNormals ? sizeof(GLfloat) * 6 * numVertices : sizeof(GLfloat) * 3 * numVertices;
//		glBufferSubData(GL_ARRAY_BUFFER, offsetTexCoords, sizeof(GLfloat) * 2 * numVertices, texCoords);
//	}
//
//	//Buffer creation with indices
//	glGenBuffers(1, &mesh->EBO);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*numIndices, indices, GL_STATIC_DRAW);
//
//	glEnableVertexAttribArray(0);
//	glVertexAttribPointer(
//		0,                  // attribute 0
//		3,                  // number of componentes (3 floats)
//		GL_FLOAT,           // data type
//		GL_FALSE,           // should be normalized?
//		0,                  // stride
//		(void*)0            // array buffer offset
//	);
//	if (hasNormals)
//	{
//		glEnableVertexAttribArray(1);
//		glVertexAttribPointer(
//			1,                  // attribute 1
//			3,                  // number of componentes (3 floats)
//			GL_FLOAT,           // data type
//			GL_FALSE,           // should be normalized?
//			0,                  // stride
//			(void*)(sizeof(float) * 3 * numVertices)       // array buffer offset
//		);
//	}
//	if (hasTexCoords)
//	{
//		glEnableVertexAttribArray(2);
//		glVertexAttribPointer(
//			2,                  // attribute 2
//			2,                  // number of componentes (2 floats)
//			GL_FLOAT,           // data type
//			GL_FALSE,           // should be normalized?
//			0,                  // stride
//			(void*)(offsetTexCoords)       // array buffer offset
//		);
//	}
//
//	// Disable VAO
//	glBindVertexArray(0);
//
//	glDisableVertexAttribArray(0);
//	glDisableVertexAttribArray(1);
//	glDisableVertexAttribArray(2);
//	
//
//	// Disable VBO and EBO
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//
//	mesh->UID = uid;
//	mesh->numIndices = numIndices;
//	mesh->vertices.reserve(numVertices);
//	for (size_t i = 0; i < numVertices; i++)
//	{
//		mesh->vertices.emplace_back(vertices);
//		vertices += 3;
//	}
//	ComputeBBox();
//	App->resManager->AddMesh(mesh);
//	delete[] data;
//}
//
//unsigned int ComponentMesh::GetMaterialIndex() const
//{
//	return materialIndex;
//}
//
//void ComponentMesh::ComputeBBox()
//{
//
//	float3 min, max;
//	min = max = mesh->vertices[0];
//
//	for (auto &vertice : mesh->vertices)
//	{
//		min.x = MIN(min.x, vertice.x);
//		min.y = MIN(min.y, vertice.y);
//		min.z = MIN(min.z, vertice.z);
//
//		max.x = MAX(max.x, vertice.x);
//		max.y = MAX(max.y, vertice.y);
//		max.z = MAX(max.z, vertice.z);
//
//	}
//	mesh->boundingBox.minPoint = min;
//	mesh->boundingBox.maxPoint = max;
//}
//
//AABB ComponentMesh::GetBoundingBox() const
//{
//	return mesh->boundingBox;
//}
//
//void ComponentMesh::DeleteBuffers()
//{
//	if (mesh != nullptr)
//	{
//		if (mesh->VAO != 0)
//		{
//			glDeleteVertexArrays(1, &mesh->VAO);
//		}
//		if (mesh->VBO != 0)
//		{
//			glDeleteBuffers(1, &mesh->VBO);
//		}
//		if (mesh->EBO != 0)
//		{
//			glDeleteBuffers(1, &mesh->EBO);
//		}
//	}
//}
//
//void ComponentMesh::Save(JSON_value * value) const
//{
//	Component::Save(value);
//	value->AddUint("UID", mesh->UID);
//}
//
//void ComponentMesh::Load(const JSON_value & value)
//{
//	Component::Load(value);
//	unsigned uid = value.GetUint("UID");
//	if (mesh != nullptr)
//	{
//		App->resManager->DeleteMesh(mesh->UID);
//	}
//	mesh = App->resManager->GetMesh(uid);
//	if (mesh == nullptr)
//	{
//		char *data;
//		App->fsystem->Load((MESHES + std::to_string(mesh->UID) + MESHEXTENSION).c_str(), &data); //TODO: use mini resource maanger to optimize this
//		SetMesh(data,uid);
//	}
//}
//
//unsigned ComponentMesh::GetUID() const
//{
//	if (mesh != nullptr)
//	{
//		return mesh->UID;
//	}
//	else return 0;
//}
//
//void ComponentMesh::SetUID(unsigned UID)
//{
//	assert(mesh != nullptr);
//	mesh->UID = UID;
//}
