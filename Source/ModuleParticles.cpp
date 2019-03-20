#include "Application.h"

#include "ModuleParticles.h"
#include "ModuleProgram.h"
#include "ModuleTextures.h"
#include "ModuleScene.h"

#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentParticles.h"
#include "ComponentCamera.h"

#include "GL/glew.h"

bool ModuleParticles::Start()
{
	float quadVertices[] =
	{
		-0.5f, -0.5f, 0.0f, // bottom left
		-0.5f,  0.5f, 0.0f, // top left 
		0.5f, -0.5f, 0.0f, // bottom right
		0.5f,  0.5f, 0.0f, // top right

		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
		1.0f, 1.0f
	};

	unsigned int quadIndices[] =
	{
		0,2,1,
		1,2,3
	};

	float quadUVS[] =
	{
		0.0f, 0.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 1.0f
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float) * 12));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	shader = App->program->CreateProgram("Particles");

	return true;
}

void ModuleParticles::Render(float dt, const ComponentCamera* camera)
{

	for (ComponentParticles* cp : particleSystems)
	{
		switch (type)
		{
		case ParticleSystemType::ANIMATION_STATIC:
			DrawAnimationStatic(cp, camera);
			break;
		}
	}
	glUseProgram(0);
}

bool ModuleParticles::CleanUp()
{
	glDeleteBuffers(1, &VAO);
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &VBO);
	return true;
}

void ModuleParticles::AddParticleSystem(ComponentParticles* cp)
{
	particleSystems.push_back(cp);
}

void ModuleParticles::DrawAnimationStatic(ComponentParticles* cp, const ComponentCamera* camera) const
{
	if (cp->texture == nullptr)
	{
		return;
	}
	glUseProgram(shader->id);
	glBindVertexArray(VAO);
	glUniformMatrix4fv(glGetUniformLocation(shader->id, "projection"), 1, GL_FALSE, &camera->GetProjectionMatrix()[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader->id, "view"), 1, GL_FALSE, &camera->GetViewMatrix()[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader->id, "model"), 1, GL_FALSE, (const float*)&cp->gameobject->transform->global);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, cp->texture->id);
	glUniform1i(glGetUniformLocation(shader->id, "texture0"), 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
