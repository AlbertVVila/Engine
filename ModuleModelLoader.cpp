#include "ModuleModelLoader.h"
#include "GL/glew.h"
#include "assert.h"

ModuleModelLoader::ModuleModelLoader()
{
}


ModuleModelLoader::~ModuleModelLoader()
{
}

bool ModuleModelLoader::Init()
{
	return true;
}


unsigned int ModuleModelLoader::Load(const char *path)
{
	const aiScene* scene = aiImportFile(path, 0);
	if (scene == NULL)
	{
		LOG("ERROR importing file:%s \n", aiGetErrorString());
	}

	for (unsigned i = 0; i < scene->mNumMeshes; ++i)
	{
		GenerateMeshData(scene->mMeshes[i]);
	}
	for (unsigned i = 0; i < scene->mNumMaterials; ++i)
	{
		GenerateMaterialData(scene->mMaterials[i]);
	}

	return 1;
}

void ModuleModelLoader::GenerateMeshData(aiMesh * mesh)
{
	unsigned vbo = 0;
	unsigned ibo = 0;
	//unsigned vao = 0;
	// Creació del Vertex Array Object (VAO) que usarem per pintar
	//glGenVertexArrays(1, &vao);
	//glBindVertexArray(vao);

	// Creació del buffer amb les dades dels vèrtexs
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*mesh->mNumVertices*5, NULL, GL_STATIC_DRAW);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * 3 * mesh->mNumVertices, mesh->mVertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * mesh->mNumVertices, sizeof(GLfloat) * 2 * mesh->mNumVertices, mesh->mTextureCoords[0]);


	//Creació buffer per indexs
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*mesh->mNumFaces*3, NULL, GL_STATIC_DRAW);
	int * pbuffer = (int*) glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER,0, sizeof(unsigned int)*mesh->mNumFaces * 3, GL_MAP_WRITE_BIT);
	for (int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		assert((int)face.mNumIndices == 3, "Num index per face is not 3");
		for (int j = 0; j < face.mNumIndices; j++)
		{
			*pbuffer++ = face.mIndices[j];
		}
	}
	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

	//Activem atribut que farem servir per vèrtex( el 0 en aquest cas)
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
	//glDrawArrays(GL_TRIANGLES, 0, mesh->mNumVertices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glDrawElements(GL_TRIANGLES, mesh->mNumFaces * 3, GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	// Desactivem el VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void ModuleModelLoader::GenerateMaterialData(aiMaterial * material)
{

}

void ModuleModelLoader::DrawModel()
{
	Load("BakerHouse.fbx");
}



bool ModuleModelLoader::CleanUp()
{
	return true;
}
