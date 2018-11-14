#include "ComponentMesh.h"
#include "Model.h"
#include "Application.h"
#include "ModuleSceneLoader.h"
#include <assert.h>
#include "GL/glew.h"

ComponentMesh::ComponentMesh(aiMesh * mesh) : Component(ComponentType::Mesh)
{
	if (mesh != nullptr)
	{
		SetMesh(mesh);
	}
}

ComponentMesh::~ComponentMesh()
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

void ComponentMesh::Draw(unsigned int shaderProgram, const Texture* texture) const
{
	glActiveTexture(GL_TEXTURE0);
	//if (App->sceneLoader->checkers)
	//{
	//	glBindTexture(GL_TEXTURE_2D, App->sceneLoader->checkersTexture.id);
	//}
	//else
	//{
	glBindTexture(GL_TEXTURE_2D, texture->id);
	//}
	glUniform1i(glGetUniformLocation(shaderProgram, "texture0"), 0);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);

	// Desactivem VAO
	glBindVertexArray(0);
	// Desactivem Textura
	glBindTexture(GL_TEXTURE_2D, 0);
}

void ComponentMesh::SetMesh(const aiMesh * mesh)
{
	assert(mesh != nullptr);

	// Creació del Vertex Array Object (VAO) que usarem per pintar
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Creació del buffer amb les dades dels vèrtexs
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*mesh->mNumVertices * 5, NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * 3 * mesh->mNumVertices, mesh->mVertices);
	float * pbuffer = (float*)glMapBufferRange(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * mesh->mNumVertices, sizeof(GLfloat) * 2 * mesh->mNumVertices, GL_MAP_WRITE_BIT);
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		*pbuffer++ = mesh->mTextureCoords[0][i].x;
		*pbuffer++ = mesh->mTextureCoords[0][i].y;

		vertices.emplace_back(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);

	//Creació buffer per indexs
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*mesh->mNumFaces * 3, NULL, GL_STATIC_DRAW);
	int * pbufferIndex = (int*)glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(unsigned int)*mesh->mNumFaces * 3, GL_MAP_WRITE_BIT);
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		assert(face.mNumIndices == 3);
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			*pbufferIndex++ = face.mIndices[j];
		}
	}
	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,                  // attribute 0
		3,                  // number of componentes (3 floats)
		GL_FLOAT,           // data type
		GL_FALSE,           // should be normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,                  // attribute 0
		2,                  // number of componentes (3 floats)
		GL_FLOAT,           // data type
		GL_FALSE,           // should be normalized?
		0,                  // stride
		(void*)(sizeof(float) * 3 * mesh->mNumVertices)       // array buffer offset
	);

	// Desactivem VAO
	glBindVertexArray(0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	

	// Desactivem el VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	numIndices = mesh->mNumFaces * 3;
	materialIndex = mesh->mMaterialIndex;
}

unsigned int ComponentMesh::GetMaterialIndex()
{
	return materialIndex;
}

