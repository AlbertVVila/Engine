#include "Application.h"

#include "ModuleParticles.h"
#include "ModuleProgram.h"
#include "ModuleTextures.h"
#include "ModuleScene.h"

#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentParticles.h"
#include "ComponentCamera.h"
#include "ComponentTrail.h"

#include "GL/glew.h"

ModuleParticles::~ModuleParticles()
{
	glDeleteBuffers(1, &billBoardVAO);
	glDeleteBuffers(1, &billBoardEBO);
	glDeleteBuffers(1, &billBoardVBO);

	glDeleteBuffers(1, &trailVAO);
	glDeleteBuffers(1, &trailEBO);
	glDeleteBuffers(1, &trailVBO);
}

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

	if (billBoardVAO == 0)
	{
		glGenVertexArrays(1, &billBoardVAO);
		glGenBuffers(1, &billBoardVBO);
		glGenBuffers(1, &billBoardEBO);
	}

	glBindVertexArray(billBoardVAO);

	glBindBuffer(GL_ARRAY_BUFFER, billBoardVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, billBoardEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float) * 12));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	shader = App->program->CreateProgram("Particles");

	if (trailVAO == 0)
	{
		glGenVertexArrays(1, &trailVAO);
		glGenBuffers(1, &trailVBO);
		glGenBuffers(1, &trailEBO);
	}

	unsigned trailIndices[MAX_TRAIL_INDICES];

	for (unsigned i = 0u; i < MAX_TRAIL_INDICES / 3; ++i)
	{
		trailIndices[i * 3] = i;
		trailIndices[i * 3 + 1] = i + 1;
		trailIndices[i * 3 + 2] = i + 2;
	}

	glBindVertexArray(trailVAO);

	glBindBuffer(GL_ARRAY_BUFFER, trailVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * MAX_TRAIL_INDICES, nullptr, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, trailEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(trailIndices), trailIndices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	//glEnableVertexAttribArray(1);
	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float) * 12));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return true;
}

void ModuleParticles::Render(float dt, const ComponentCamera* camera)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	for (ComponentParticles* cp : particleSystems)
	{
		cp->gameobject->transform->LookAt(camera->frustum->pos);
		cp->Update(dt);
		switch (type)
		{
		case ParticleSystemType::ANIMATION_STATIC:
			DrawAnimationStatic(cp, camera);
			break;
		}
	}

	for (ComponentTrail* trail : trails)
	{
		if (trail->trail.size() > 1)
		{
			RenderTrail(trail, camera);
		}
	}
	glDisable(GL_BLEND);
	glUseProgram(0);
}

void ModuleParticles::RenderTrail(ComponentTrail* ct, const ComponentCamera* camera)
{
	glUseProgram(shader->id);
	glBindVertexArray(trailVAO);
	glBindBuffer(GL_ARRAY_BUFFER, trailVBO);
	unsigned trailVertices = ct->trail.size();

	void *ptr = glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(math::float3) * trailVertices * 2,
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

	for (unsigned i = 0u; i < trailVertices; ++i)
	{
		trailData[i * 2] = ct->trail.front().leftPoint;
		trailData[i * 2 + 1] = ct->trail.front().rightPoint;
		ct->trail.push(ct->trail.front());
		ct->trail.pop();
	}
	memcpy(ptr, trailData, sizeof(math::float3) * ct->trail.size() * 2);
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, trailVBO);

	glUniformMatrix4fv(glGetUniformLocation(shader->id, "projection"), 1, GL_FALSE, &camera->GetProjectionMatrix()[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader->id, "view"), 1, GL_FALSE, &camera->GetViewMatrix()[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader->id, "model"), 1, GL_TRUE, float4x4::identity.ptr());

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, billBoardEBO);
	glDrawElements(GL_TRIANGLES, (ct->trail.size() * 2), GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

bool ModuleParticles::CleanUp()
{	
	particleSystems.clear();
	return true;
}

void ModuleParticles::AddParticleSystem(ComponentParticles* cp)
{
	particleSystems.push_back(cp);
}

void ModuleParticles::AddTrailRenderer(ComponentTrail* cr)
{
	trails.push_back(cr);
}

void ModuleParticles::RemoveTrailRenderer(ComponentTrail* cr)
{
	trails.remove(cr);
}

void ModuleParticles::DrawAnimationStatic(ComponentParticles* cp, const ComponentCamera* camera) const
{
	if (cp->texture == nullptr)
	{
		return;
	}
	glUseProgram(shader->id);
	glBindVertexArray(billBoardVAO);
	glUniformMatrix4fv(glGetUniformLocation(shader->id, "projection"), 1, GL_FALSE, &camera->GetProjectionMatrix()[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader->id, "view"), 1, GL_FALSE, &camera->GetViewMatrix()[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader->id, "model"), 1, GL_TRUE, (const float*)&cp->gameobject->transform->global);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, cp->texture->id);
	glUniform1i(glGetUniformLocation(shader->id, "texture0"), 0);
	glUniform1i(glGetUniformLocation(shader->id, "xTiles"), cp->xTiles);
	glUniform1i(glGetUniformLocation(shader->id, "yTiles"), cp->yTiles);
	glUniform1i(glGetUniformLocation(shader->id, "f1Xpos"), cp->f1Xpos);
	glUniform1i(glGetUniformLocation(shader->id, "f1Ypos"), cp->f1Ypos);
	glUniform1i(glGetUniformLocation(shader->id, "f2Xpos"), cp->f2Xpos);
	glUniform1i(glGetUniformLocation(shader->id, "f2Ypos"), cp->f2Ypos);
	glUniform1f(glGetUniformLocation(shader->id, "mixAmount"), cp->frameMix);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, billBoardEBO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
