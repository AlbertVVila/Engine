#include "Mesh.h"


Mesh::Mesh(aiMesh * mesh)
{
	//unsigned vao = 0;
	// Creaci� del Vertex Array Object (VAO) que usarem per pintar
	//glGenVertexArrays(1, &vao);
	//glBindVertexArray(vao);

	// Creaci� del buffer amb les dades dels v�rtexs
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*mesh->mNumVertices * 5, NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * 3 * mesh->mNumVertices, mesh->mVertices);
	float * pbuffer = (float*)glMapBufferRange(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * mesh->mNumVertices, sizeof(GLfloat) * 2 * mesh->mNumVertices, GL_MAP_WRITE_BIT);
	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		*pbuffer++ = mesh->mTextureCoords[0][i].x;
		*pbuffer++ = mesh->mTextureCoords[0][i].y;
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);

	//Creaci� buffer per indexs
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*mesh->mNumFaces * 3, NULL, GL_STATIC_DRAW);
	int * pbufferIndex = (int*)glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(unsigned int)*mesh->mNumFaces * 3, GL_MAP_WRITE_BIT);
	for (int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		assert((int)face.mNumIndices == 3, "Num index per face is not 3");
		for (int j = 0; j < face.mNumIndices; j++)
		{
			*pbufferIndex++ = face.mIndices[j];
		}
	}
	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

	numVertices = mesh->mNumVertices;
	numIndices = mesh->mNumFaces * 3;
	materialIndex = mesh->mMaterialIndex;
}

Mesh::~Mesh()
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

void Mesh::Draw(unsigned int shaderProgram, const std::vector<unsigned int> &textures) const
{
	//we will have to loop textures adding uniforms
	//first we only get one texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[materialIndex]);
	glUniform1i(glGetUniformLocation(shaderProgram, "texture0"), 0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//Activem atribut que farem servir per v�rtex( el 0 en aquest cas)
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
		(void*)(sizeof(float) * 3 * numVertices)       // array buffer offset
	);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	// Desactivem el VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Desactivem Textura
	glBindTexture(GL_TEXTURE_2D, 0);
}
