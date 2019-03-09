#include "ResourceSkybox.h"

#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleResourceManager.h"
#include "ModuleFileSystem.h"
#include "ModuleProgram.h"

#include "ResourceTexture.h"

#include "Brofiler.h"
#include "Math/float4x4.h"
#include "Geometry/Frustum.h"
#include "GL/glew.h"
#include "JSON.h"

#define SKYBOX_SHADER "Skybox"

ResourceSkybox::ResourceSkybox(unsigned uid) : Resource(uid, TYPE::SKYBOX)
{
	shader = App->program->GetProgram(SKYBOX_SHADER);
}

ResourceSkybox::ResourceSkybox(unsigned uid, std::string faces[NUMFACES]) : Resource(uid, TYPE::SKYBOX)
{
	for (unsigned int i = 0; i < NUMFACES; ++i)
	{
		textures[i] = (ResourceTexture*)App->resManager->Get(faces[i].c_str());
		textures[i]->imageType = IMAGE_TYPE::CUBEMAP;
		textures[i]->cubemapIndex = i;
	}

	shader = App->program->GetProgram(SKYBOX_SHADER);
}


ResourceSkybox::~ResourceSkybox()
{
	DeleteFromMemory();
}

bool ResourceSkybox::LoadInMemory()
{
	skybox_cubemap = LoadCubeMap(textures);

	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glBindVertexArray(0);
	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	++loaded;

	return true;
}

void ResourceSkybox::DeleteFromMemory()
{
	Resource::DeleteFromMemory();
	//Delete used textures


	if (skybox_cubemap != 0)
	{
		glDeleteTextures(1, &skybox_cubemap);
	}
	if (skyboxVAO != 0)
	{
		glDeleteBuffers(1, &skyboxVAO);
	}
	if (skyboxVBO != 0)
	{
		glDeleteBuffers(1, &skyboxVBO);
	}
}

void ResourceSkybox::SetTextures(std::string faces[NUMFACES])
{
	for (unsigned int i = 0; i < NUMFACES; ++i)
	{
		textures[i] = (ResourceTexture*)App->resManager->GetWithoutLoad(faces[i].c_str());
		textures[i]->imageType = IMAGE_TYPE::CUBEMAP;
		textures[i]->cubemapIndex = i;
	}
}

void ResourceSkybox::Draw(const Frustum& frustum) const
{
	PROFILE;
	if (!enabled) return;

	glDepthMask(GL_FALSE);

	glUseProgram(shader->id);
	glBindVertexArray(skyboxVAO);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_cubemap);
	float4x4 model(float4x4::FromTRS(frustum.pos,
		float4x4::identity, float3::one));
	glUniformMatrix4fv(glGetUniformLocation(shader->id,
		"model"), 1, GL_TRUE, &model[0][0]);

	glUniform1i(glGetUniformLocation(shader->id, "skybox"), 0);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glUseProgram(0);

	glDepthMask(GL_TRUE);
}

unsigned ResourceSkybox::LoadCubeMap(ResourceTexture* faces[]) const
{
	unsigned textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	unsigned width = 0;
	unsigned height = 0;
	unsigned pixelDepth = 0;
	int format = 0;

	for (unsigned int i = 0; i < NUMFACES; ++i)
	{
		faces[i]->SetReferences(faces[i]->GetReferences() + 1u);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}