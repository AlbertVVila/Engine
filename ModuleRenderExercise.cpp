#include "Application.h"

#include "ModuleRenderExercise.h"
#include "ModuleWindow.h"

#include "GL/glew.h"
#include "SDL.h"

ModuleRenderExercise::ModuleRenderExercise()
{
}

ModuleRenderExercise::~ModuleRenderExercise()
{
}

bool ModuleRenderExercise::Init()
{
    float vertex_buffer_data[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        0.0f,  1.0f, 0.0f,
	};

	math::float4 v1(vertex_buffer_data[0], vertex_buffer_data[1], vertex_buffer_data[2], 1);
	math::float4 v2(vertex_buffer_data[3], vertex_buffer_data[4], vertex_buffer_data[5], 1);
	math::float4 v3(vertex_buffer_data[6], vertex_buffer_data[7], vertex_buffer_data[8], 1);

	math::float4x4 transform = ProjectionMatrix() * ViewMatrix() * ModelMatrix();

	v1 = transform*v1;
	v2 = transform*v2;
	v3 = transform*v3;

	math::float3 r1(v1.x / v1.w, v1.y / v1.w, v1.z / v1.w);
	math::float3 r2(v2.x / v2.w, v2.y / v2.w, v2.z / v2.w);
	math::float3 r3(v3.x / v3.w, v3.y / v3.w, v3.z / v3.w);

	float trans_vertex_buffer[] = {
		r1.x, r1.y, r1.z,
		r2.x, r2.y, r2.z,
		r3.x, r3.y, r3.z,
	};

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(trans_vertex_buffer), trans_vertex_buffer, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return vbo;
}

update_status ModuleRenderExercise::Update()
{
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(
            0,                  // attribute 0
            3,                  // number of componentes (3 floats)
            GL_FLOAT,           // data type
            GL_FALSE,           // should be normalized?
            0,                  // stride
            (void*)0            // array buffer offset
            );

    glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle

    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

	return UPDATE_CONTINUE;
}

bool ModuleRenderExercise::CleanUp()
{
    if(vbo != 0)
    {
        glDeleteBuffers(1, &vbo);
    }

	return true;
}

math::float4x4 ModuleRenderExercise::ViewMatrix()
{
	return LookAt(math::float3(0,0,5), math::float3(0, 0, 0), math::float3(0, 1, 0));
}

math::float4x4 ModuleRenderExercise::LookAt(math::float3 OBS, math::float3 VRP, math::float3 up)
{
	math::float3 f(VRP - OBS); f.Normalize();
	math::float3 s(f.Cross(up)); f.Normalize();
	math::float3 u(s.Cross(f));

	math::float4x4 matrix(math::float4x4::zero);
	matrix[0][0] = s.x; matrix[0][1] = s.y; matrix[0][2] = s.z;
	matrix[1][0] = u.x; matrix[1][1] = u.y; matrix[1][2] = u.z;
	matrix[2][0] = -f.x; matrix[2][1] = -f.y; matrix[2][2] = -f.z;
	matrix[0][3] = -s.Dot(OBS); matrix[1][3] = -u.Dot(OBS); matrix[2][3] = f.Dot(OBS);
	matrix[3][3] = 1;

	return matrix;
}


math::float4x4 ModuleRenderExercise::ProjectionMatrix()
{
	Frustum frustum;
	frustum.type = FrustumType::PerspectiveFrustum;
	frustum.pos = float3::zero;
	frustum.front = -float3::unitZ;
	frustum.up = float3::unitY;
	frustum.nearPlaneDistance = 0.1f;
	frustum.farPlaneDistance = 100.0f;
	frustum.verticalFov = math::pi / 4.0f;
	frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov * 0.5f) * (SCREEN_WIDTH/SCREEN_HEIGHT));
	return frustum.ProjectionMatrix();
}

math::float4x4 ModuleRenderExercise::ModelMatrix()
{
	return math::float4x4::identity;
}


