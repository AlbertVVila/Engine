#include "ModuleModelLoader.h"
#include "GL/glew.h"
#include "assert.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleProgram.h"

ModuleModelLoader::ModuleModelLoader()
{
}


ModuleModelLoader::~ModuleModelLoader()
{
}

bool ModuleModelLoader::Init()
{
	//Load("BakerHouse.fbx");
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
	// Creaci� del Vertex Array Object (VAO) que usarem per pintar
	//glGenVertexArrays(1, &vao);
	//glBindVertexArray(vao);

	// Creaci� del buffer amb les dades dels v�rtexs
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*mesh->mNumVertices*5, NULL, GL_STATIC_DRAW);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * 3 * mesh->mNumVertices, mesh->mVertices);
	/*glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * mesh->mNumVertices, sizeof(GLfloat) * 2 * mesh->mNumVertices, NULL);*/
	float * pbuffer = (float*)glMapBufferRange(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * mesh->mNumVertices, sizeof(GLfloat) * 2 * mesh->mNumVertices, GL_MAP_WRITE_BIT);
	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		*pbuffer++ = mesh->mTextureCoords[0][i].x;
		*pbuffer++ = mesh->mTextureCoords[0][i].y;
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);

	//Creaci� buffer per indexs
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*mesh->mNumFaces*3, NULL, GL_STATIC_DRAW);
	int * pbufferIndex = (int*) glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER,0, sizeof(unsigned int)*mesh->mNumFaces * 3, GL_MAP_WRITE_BIT);
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


	meshVBO.push_back(vbo);
	meshNumVertices.push_back(mesh->mNumVertices);
	meshNumIndices.push_back(mesh->mNumFaces * 3);
	matIndices.push_back(mesh->mMaterialIndex);
	meshEBO.push_back(ibo);
}

void ModuleModelLoader::GenerateMaterialData(aiMaterial * material)
{
	aiTextureMapping mapping = aiTextureMapping_UV;
	aiString file("Baker_house.png");
	material->GetTexture(aiTextureType_DIFFUSE, 0, &file, &mapping, 0);
	unsigned int texture0 = App->textures->Load("Baker_house.png");
	MatTexture.push_back(texture0);
}

void ModuleModelLoader::DrawModel()
{
	for (int i = 0; i < meshVBO.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, MatTexture[matIndices[i]]);
		glUniform1i(glGetUniformLocation(App->program->shaderProgram, "texture0"), 0);

		glBindBuffer(GL_ARRAY_BUFFER, meshVBO[i]);
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
			(void*)(sizeof(float) * 3 *meshNumVertices[i])       // array buffer offset
		);
		//glDrawArrays(GL_TRIANGLES, 0, mesh->mNumVertices);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshEBO[i]);
		glDrawElements(GL_TRIANGLES, meshNumIndices[i], GL_UNSIGNED_INT, 0);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		// Desactivem el VBO
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}



bool ModuleModelLoader::CleanUp()
{
	return true;
}
