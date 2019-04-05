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

#include "ImGUICurveUtils.h"

ModuleParticles::~ModuleParticles()
{
	glDeleteBuffers(1, &billBoardVAO);
	glDeleteBuffers(1, &billBoardEBO);
	glDeleteBuffers(1, &billBoardVBO);
	glDeleteBuffers(1, &billBoardInstanceVBO);

	glDeleteBuffers(1, &trailVAO);
	glDeleteBuffers(1, &trailVBO);
}

bool ModuleParticles::Start()
{
	float quadVertices[] =
	{
		- 0.5f, -0.5f, 0.0f, // bottom left 0
		0.5f, -0.5f, 0.0f, // bottom right 2
		-0.5f,  0.5f, 0.0f, // top left 1

		-0.5f,  0.5f, 0.0f, // top left 1
		0.5f, -0.5f, 0.0f, // bottom right 2
		0.5f,  0.5f, 0.0f, // top right 3

		0.0f, 0.0f, // 0
		1.0f, 0.0f, // 2
		0.0f, 1.0f, // 1

		0.0f, 1.0f, // 1
		1.0f, 0.0f, // 2
		1.0f, 1.0f  // 3
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
		glGenBuffers(1, &billBoardInstanceVBO);
		glGenBuffers(1, &billBoardEBO);
	}

	glBindVertexArray(billBoardVAO);

	glBindBuffer(GL_ARRAY_BUFFER, billBoardVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float) * 18));

	glBindBuffer(GL_ARRAY_BUFFER, billBoardInstanceVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 16 * MAX_PARTICLES, nullptr, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 16, (void*)(0));
	glVertexAttribDivisor(2, 1);

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 16, (void*)(sizeof(float) * 4));
	glVertexAttribDivisor(3, 1);

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 16, (void*)(sizeof(float) * 8));
	glVertexAttribDivisor(4, 1);

	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 16, (void*)(sizeof(float) * 12));
	glVertexAttribDivisor(5, 1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	shader = App->program->CreateProgram("Particles");
	trailShader = App->program->CreateProgram("Trail");

	if (trailVAO == 0)
	{
		glGenVertexArrays(1, &trailVAO);
		glGenBuffers(1, &trailVBO);
	}

	glBindVertexArray(trailVAO);

	glBindBuffer(GL_ARRAY_BUFFER, trailVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * MAX_TRAIL_VERTICES * 3 * 2, nullptr, GL_DYNAMIC_DRAW); //vert. 3 float + 2 float uvs

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(sizeof(float) * 3));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return true;
}

void ModuleParticles::Render(float dt, const ComponentCamera* camera) const
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	for (ComponentParticles* cp : particleSystems)
	{
		cp->gameobject->transform->LookAt(camera->frustum->pos);
		cp->Update(dt);
		switch (cp->type)
		{
		case ComponentParticles::ParticleSystemType::ANIMATION_STATIC:
			DrawAnimationStatic(cp, camera);
			break;
		}
	}
	glDisable(GL_CULL_FACE);
	for (ComponentTrail* trail : trails)
	{
		if (trail->trail.size() > 1)
		{
			RenderTrail(trail, camera);
		}
	}
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);

	glUseProgram(0);
}

void ModuleParticles::RenderTrail(ComponentTrail* ct, const ComponentCamera* camera) const
{
	if (ct->texture == nullptr)
	{
		return;
	}

	glUseProgram(trailShader->id);
	glBindVertexArray(trailVAO);
	unsigned trailVertices = ct->trail.size();

	glBindBuffer(GL_ARRAY_BUFFER, trailVBO);
	float* ptrVertices = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(float) * MAX_TRAIL_VERTICES * 6, GL_MAP_WRITE_BIT);

	math::float3 P0L;
	math::float3 P0R;

	unsigned discarded = 0u;

	float sPortion = 1 / (float)trailVertices;

	for (unsigned i = 0u; i < trailVertices; ++i)
	{
		TrailPoint point = ct->trail.front();
		if (point.renderizable)
		{
			float width = point.width;
			for (ParticleModule* pm : ct->modules)
			{
				switch (pm->type)
				{
				case ParticleModule::ParticleModulesType::SIZE_OVER_TIME:
					width = ((PMSizeOverTime*)pm)->GetSize(point.remainingTime / point.totalTime, point.width);
					break;
				}
			}
			P0L = point.position + point.rightPoint * width;
			P0R = point.position - point.rightPoint * width;
			memcpy(ptrVertices, &P0L.x, sizeof(float) * 3); ptrVertices += 3;
			memcpy(ptrVertices, &math::float2(sPortion * i, 0.f), sizeof(float) * 2); ptrVertices += 2;
			memcpy(ptrVertices, &P0R.x, sizeof(float) * 3);	ptrVertices += 3;
			memcpy(ptrVertices, &math::float2(sPortion * i, 1), sizeof(float) * 2); ptrVertices += 2;
		}
		else
		{
			++discarded;
		}
		ct->trail.push(ct->trail.front());
		ct->trail.pop();
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);

	glUniformMatrix4fv(glGetUniformLocation(trailShader->id, "projection"), 1, GL_FALSE, &camera->GetProjectionMatrix()[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(trailShader->id, "view"), 1, GL_FALSE, &camera->GetViewMatrix()[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(trailShader->id, "model"), 1, GL_TRUE, float4x4::identity.ptr());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ct->texture->id);
	glUniform1i(glGetUniformLocation(shader->id, "texture0"), 0);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, (trailVertices - discarded) * 2);

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
	glBindBuffer(GL_ARRAY_BUFFER, billBoardInstanceVBO);
	float* matrices = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

	//REPEAT FOR EACH PARTICLE
	memcpy(matrices, &cp->gameobject->transform->global.Col(0), sizeof(float) * 4); matrices += 4;
	memcpy(matrices, &cp->gameobject->transform->global.Col(1), sizeof(float) * 4); matrices += 4;
	memcpy(matrices, &cp->gameobject->transform->global.Col(2), sizeof(float) * 4); matrices += 4;
	memcpy(matrices, &cp->gameobject->transform->global.Col(3), sizeof(float) * 4); matrices += 4;

	//

	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glUniformMatrix4fv(glGetUniformLocation(shader->id, "projection"), 1, GL_FALSE, &camera->GetProjectionMatrix()[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader->id, "view"), 1, GL_FALSE, &camera->GetViewMatrix()[0][0]);
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

	glDrawArraysInstanced(GL_TRIANGLES,0, 6, 1);

	glBindVertexArray(0);

}

inline float PMSizeOverTime::GetSize(float percent, float total)
{
	return ImGui::BezierValue(percent, v) * total;
}

void PMSizeOverTime::InspectorDraw()
{
	ImGui::Text("Size Over Time");
	ImGui::Bezier("easeInExpo", v);
}
