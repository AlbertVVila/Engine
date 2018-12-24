#include "ComponentMesh.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleFileSystem.h"
#include <assert.h>
#include "GL/glew.h"
#include "Imgui/imgui.h"
#include "JSON.h"

ComponentMesh::ComponentMesh(GameObject* gameobject, char * mesh) : Component(gameobject, ComponentType::Mesh)
{
	if (mesh != nullptr)
	{
		SetMesh(mesh);
	}
}

ComponentMesh::ComponentMesh(const ComponentMesh & component) : Component(component) //TODO: SaveMesh (for sphere, cube, etc)
{
	//TODO: gen new buffers
	VAO = component.VAO;
	VBO = component.VBO;
	EBO = component.EBO;
	materialIndex = component.materialIndex;
	numIndices = component.numIndices;
	vertices.reserve(component.vertices.capacity());
	vertices = component.vertices;
	boundingBox = component.boundingBox;
	meshUID = component.meshUID;
}

ComponentMesh::~ComponentMesh()
{
	DeleteBuffers(); //TODO: clean up components or simply call destructor?
}

Component * ComponentMesh::Clone() const
{
	return new ComponentMesh(*this);
}

void ComponentMesh::Draw(unsigned int shaderProgram) const
{
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);

	// Desactivem VAO
	glBindVertexArray(0);
	// Desactivem Textura
}

void ComponentMesh::DrawProperties()
{
	ImGui::PushID(this);
	if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen))
	{
		bool removed = Component::DrawComponentState();
		if (removed)
		{
			ImGui::PopID();
			return;
		}
		ImGui::Text("Num vertices : %d", vertices.size());
		ImGui::Text("Num triangles : %d", numIndices / 3);
		ImGui::Separator();
	}
	ImGui::PopID();
}

void ComponentMesh::SetMesh(char * &mesh) //TODO: pass by reference or know size of mesh
{
	assert(mesh != nullptr);
	DeleteBuffers();
	char *data = mesh;

	unsigned int numIndices = *(int*)mesh;
	mesh += sizeof(int);

	unsigned int numVertices = *(int*)mesh;
	mesh += sizeof(int);

	float* vertices = (float*)mesh;
	mesh += sizeof(float) * 3 * numVertices;
	
	bool hasNormals = (bool*)mesh;
	mesh += sizeof(bool);

	float* normals = nullptr;
	if (hasNormals)
	{
		normals = (float*)mesh;
		mesh += sizeof(float) * 3 * numVertices;
	}

	bool hasTexCoords = (bool*)mesh;
	mesh += sizeof(bool);

	float* texCoords = nullptr;
	if (hasTexCoords)
	{
		texCoords = (float*)mesh;
		mesh += sizeof(float) * 2 * numVertices;
	}

	int* indices = (int*)mesh;
	mesh += sizeof(int) * numIndices;
	// VAO Creation
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Buffer Creation with vertex data
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	unsigned bufferSize = sizeof(GLfloat)*numVertices * 3;
	if (hasNormals) bufferSize += sizeof(GLfloat)*numVertices * 3;
	if (hasTexCoords) bufferSize+= sizeof(GLfloat)*numVertices * 2;
	glBufferData(GL_ARRAY_BUFFER, bufferSize, NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * 3 * numVertices, vertices);

	if (hasNormals)
	{
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * numVertices, sizeof(GLfloat) * 3 * numVertices, normals);
	}
	unsigned offsetTexCoords = 0;
	if (hasTexCoords)
	{
		offsetTexCoords = hasNormals ? sizeof(GLfloat) * 6 * numVertices : sizeof(GLfloat) * 3 * numVertices;
		glBufferSubData(GL_ARRAY_BUFFER, offsetTexCoords, sizeof(GLfloat) * 2 * numVertices, texCoords);
	}

	//Buffer creation with indices
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*numIndices, indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,                  // attribute 0
		3,                  // number of componentes (3 floats)
		GL_FLOAT,           // data type
		GL_FALSE,           // should be normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	if (hasNormals)
	{
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(
			1,                  // attribute 1
			3,                  // number of componentes (3 floats)
			GL_FLOAT,           // data type
			GL_FALSE,           // should be normalized?
			0,                  // stride
			(void*)(sizeof(float) * 3 * numVertices)       // array buffer offset
		);
	}
	if (hasTexCoords)
	{
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(
			2,                  // attribute 2
			2,                  // number of componentes (2 floats)
			GL_FLOAT,           // data type
			GL_FALSE,           // should be normalized?
			0,                  // stride
			(void*)(offsetTexCoords)       // array buffer offset
		);
	}

	// Disable VAO
	glBindVertexArray(0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	

	// Disable VBO and EBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	this->numIndices = numIndices;
	this->vertices.reserve(numVertices);
	for (size_t i = 0; i < numVertices; i++)
	{
		this->vertices.emplace_back(vertices);
		vertices += 3;
	}
	//materialIndex = mesh->mMaterialIndex; TODO import/load materials
	ComputeBBox();
	delete[] data;
}

unsigned int ComponentMesh::GetMaterialIndex()
{
	return materialIndex;
}

void ComponentMesh::ComputeBBox()
{

	float3 min, max;
	min = max = vertices[0];

	for (auto &vertice : vertices)
	{
		min.x = MIN(min.x, vertice.x);
		min.y = MIN(min.y, vertice.y);
		min.z = MIN(min.z, vertice.z);

		max.x = MAX(max.x, vertice.x);
		max.y = MAX(max.y, vertice.y);
		max.z = MAX(max.z, vertice.z);

	}
	boundingBox.minPoint = min;
	boundingBox.maxPoint = max;
}

AABB ComponentMesh::GetBoundingBox() const
{
	return boundingBox;
}

void ComponentMesh::DeleteBuffers()
{
	if (VAO != 0)
	{
		glDeleteVertexArrays(1, &VAO);
	}
	if (VBO != 0)
	{
		glDeleteBuffers(1, &VBO);
	}
	if (EBO != 0)
	{
		glDeleteBuffers(1, &EBO);
	}
}

void ComponentMesh::Save(JSON_value * value) const
{
	Component::Save(value);
	value->AddUint("UID", meshUID);
}

void ComponentMesh::Load(const JSON_value & value)
{
	Component::Load(value);
	meshUID = value.GetUint("UID");
	char *data;
	App->fsystem->Load((MESHES + std::to_string(meshUID) + MESHEXTENSION).c_str(), &data); //TODO: use mini resource maanger to optimize this
	SetMesh(data);
}
