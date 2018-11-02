#include "Application.h"
#include "ModuleProgram.h"
#include "ModuleRenderExercise.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleModelLoader.h"
#include "ModuleRender.h"
#include "ModuleTextures.h"
#include "ModuleTime.h"
#include "GL/glew.h"
#include "IL/ilut.h"
#include "SDL.h"
#include "ModuleEditor.h"

ModuleRenderExercise::ModuleRenderExercise()
{
}

ModuleRenderExercise::~ModuleRenderExercise()
{
}

bool ModuleRenderExercise::Init()
{
	frustum.type = FrustumType::PerspectiveFrustum;
	frustum.pos = float3::zero;
	frustum.front = -float3::unitZ;
	frustum.up = float3::unitY;
	frustum.nearPlaneDistance = 0.1f;
	frustum.farPlaneDistance = 100.0f;
	frustum.verticalFov = math::pi / 4.0f;
	frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov * 0.5f) * ((float)App->renderer->width / (float)App->renderer->height));

	CreateBuffers();
	texture0 = App->textures->Load(image);
    return true;
}

update_status ModuleRenderExercise::Update()
{
	ProcessInput();

	glUseProgram(App->program->shaderProgram);
	ModelTransform();
	ProjectionMatrix();
	ViewMatrix();

	float white[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glUniform4fv(glGetUniformLocation(App->program->shaderProgram,
		"Vcolor"), 1, white);


	App->loader->DrawModel();
	DrawLines();
	DrawAxis();

	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
	return UPDATE_CONTINUE;
}

void ModuleRenderExercise::CreateBuffers()
{
	float vertex_buffer_data[] = { 
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
	  - 1.0f, 1.0f, 0.0f,
		
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f
	
	};

	// Creació del Vertex Array Object (VAO) que usarem per pintar
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Creació del buffer amb les dades dels vèrtexs
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

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
		(void*)(sizeof(float) * 3 * 6)           // array buffer offset
	);

	// Desactivem el VAO
	glBindVertexArray(0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	// Desactivem el VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ModuleRenderExercise::DrawLines()
{

	float white[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glUniform4fv(glGetUniformLocation(App->program->shaderProgram,
		"Vcolor"), 1, white);

	glLineWidth(1.0f);
	float d = 200.0f;
	glBegin(GL_LINES);

	for (float i = -d; i <= d; i += 1.0f)
	{
		glVertex3f(i, 0.0f, -d);
		glVertex3f(i, 0.0f, d);
		glVertex3f(-d, 0.0f, i);
		glVertex3f(d, 0.0f, i);
	}
	glEnd();
}

void ModuleRenderExercise::DrawAxis()
{
	glLineWidth(2.0f);

	float red[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	glUniform4fv(glGetUniformLocation(App->program->shaderProgram,
		"Vcolor"), 1, red);

	glBegin(GL_LINES);
	// red X

	glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 0.1f, 0.0f); glVertex3f(1.1f, -0.1f, 0.0f);
	glVertex3f(1.1f, 0.1f, 0.0f); glVertex3f(1.0f, -0.1f, 0.0f);
	glEnd;

	float green[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
	glUniform4fv(glGetUniformLocation(App->program->shaderProgram,
		"Vcolor"), 1, green);

	glBegin(GL_LINES);
	// green Y
	glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
	glVertex3f(0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
	glVertex3f(0.0f, 1.15f, 0.0f); glVertex3f(0.0f, 1.05f, 0.0f);
	glEnd();


	float blue[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
	glUniform4fv(glGetUniformLocation(App->program->shaderProgram,
		"Vcolor"), 1, blue);

	glBegin(GL_LINES);
	// blue Z
	glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-0.05f, 0.1f, 1.05f); glVertex3f(0.05f, 0.1f, 1.05f);
	glVertex3f(0.05f, 0.1f, 1.05f); glVertex3f(-0.05f, -0.1f, 1.05f);
	glVertex3f(-0.05f, -0.1f, 1.05f); glVertex3f(0.05f, -0.1f, 1.05f);
	glEnd();

	glLineWidth(1.0f);
}

bool ModuleRenderExercise::CleanUp()
{
    if(vbo != 0)
    {
        glDeleteBuffers(1, &vbo);
    }

	return true;
}

void ModuleRenderExercise::ProcessInput()
{
	if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT)
	{
		cameraPos.y += cameraSpeed * App->time->deltaTime;
	}
	if (App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT)
	{
		cameraPos.y -= cameraSpeed * App->time->deltaTime;
	}
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
	{
		cameraPos -= cameraFront * cameraSpeed * App->time->deltaTime;
	}
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
	{
		cameraPos += cameraFront * cameraSpeed * App->time->deltaTime;
	}
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		cameraPos -= cameraFront.Cross(cameraUp).Normalized() * cameraSpeed * App->time->deltaTime;
	}
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		cameraPos += cameraFront.Cross(cameraUp).Normalized() * cameraSpeed * App->time->deltaTime;
	}
	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
	{
		pitch= MIN(89,pitch+cameraSpeed * App->time->deltaTime * 20);
		ComputeEulerAngles();
	}
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		pitch= MAX(-89, pitch - cameraSpeed * App->time->deltaTime * 20);
		ComputeEulerAngles();
	}
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		yaw -= cameraSpeed * App->time->deltaTime * 20;
		ComputeEulerAngles();
	}
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		yaw += cameraSpeed * App->time->deltaTime * 20;
		ComputeEulerAngles();
	}
	if (App->input->GetKey(SDL_SCANCODE_T) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_T) == KEY_REPEAT)
	{
		frustum.verticalFov-=.5f*App->time->deltaTime;
		frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov * 0.5f) * ((float)App->renderer->width / (float)App->renderer->height));
	}
}

math::float4x4 ModuleRenderExercise::LookAt(math::float3 OBS, math::float3 VRP, math::float3 up)
{
	math::float3 forward(VRP - OBS); forward.Normalize(); //deprecated with camerafront pos and up
	math::float3 side(forward.Cross(up)); side.Normalize();
	math::float3 u(side.Cross(forward));

	math::float4x4 matrix(math::float4x4::zero);
	matrix[0][0] = side.x; matrix[0][1] = side.y; matrix[0][2] = side.z;
	matrix[1][0] = u.x; matrix[1][1] = u.y; matrix[1][2] = u.z;
	matrix[2][0] = -forward.x; matrix[2][1] = -forward.y; matrix[2][2] = -forward.z;
	matrix[0][3] = -side.Dot(OBS); matrix[1][3] = -u.Dot(OBS); matrix[2][3] = forward.Dot(OBS);
	matrix[3][3] = 1;

	return matrix;
}

void ModuleRenderExercise::ViewMatrix()
{
	math::float4x4 view = LookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	glUniformMatrix4fv(glGetUniformLocation(App->program->shaderProgram,
		"view"), 1, GL_TRUE, &view[0][0]);
}

void ModuleRenderExercise::ProjectionMatrix()
{
	glUniformMatrix4fv(glGetUniformLocation(App->program->shaderProgram,
		"proj"), 1, GL_TRUE, &frustum.ProjectionMatrix()[0][0]);
}

void ModuleRenderExercise::ModelTransform()
{
	math::float4x4 model =  math::float4x4::identity;
	glUniformMatrix4fv(glGetUniformLocation(App->program->shaderProgram,
		"model"), 1, GL_TRUE, &model[0][0]);
}

void ModuleRenderExercise::ComputeEulerAngles()
{
	cameraFront.x = cos(math::DegToRad(yaw)) * cos(math::DegToRad(pitch));
	cameraFront.y = sin(math::DegToRad(pitch));
	cameraFront.z = sin(math::DegToRad(yaw)) *cos(math::DegToRad(pitch));
	cameraFront.Normalize();

}

void ModuleRenderExercise::ShowRenderExerciseDialog()
{
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar;

	ImVec2 main_viewport_pos = ImGui::GetMainViewport()->Pos;
	ImGui::SetNextWindowPos(ImVec2(main_viewport_pos.x + 250, main_viewport_pos.y + 20), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin("Texture Editor", false, window_flags))
	{
		ImGui::End();
		return;
	}

	ImGui::Text("IMAGES:");
	static int e = 0;
	ImGui::RadioButton("Lenna", &e, 0); ImGui::SameLine();
	ImGui::RadioButton("Black Hole", &e, 1); ImGui::SameLine();
	ImGui::RadioButton("Not Blitzcrank", &e, 2);

	char* newImage;
	switch (e)
	{
	case 0:
		newImage = "Lenna.png";
		break;
	case 1:
		newImage = "black_hole.jpg";
		break;
	case 2:
		newImage = "not_blitzcrank.jpg";
		break;
	default:
		break;
	}
	if (newImage != image)
	{
		texture0 = App->textures->Load(newImage);
		image = newImage;
	}
	ImGui::Separator();
	ImGui::Text("CURRENT IMAGE INFO:"); 

	ImGui::Text("Width: %dpx",App->textures->width); 
	ImGui::Text("Height: %dpx", App->textures->height);
	ImGui::Text("Pixel depth: %d", App->textures->pixelDepth);
	ImGui::Text("Format: %d", App->textures->format);

	if (ImGui::TreeNode("Image Options"))
	{
		ImGui::Checkbox("Mipmap", &App->textures->mipmap);
		ImGui::Checkbox("MAG filter", &App->textures->magfilter);
		ImGui::Checkbox("MIN filter", &App->textures->minfilter);
		ImGui::Checkbox("Anisotropic Filtering", &App->textures->anisotropic_filter);

		ImGui::Text("WRAP MODE:");
		ImGui::RadioButton("Clamp", &App->textures->wrap_mode, 0); ImGui::SameLine();
		ImGui::RadioButton("Clamp to Border", &App->textures->wrap_mode, 1);
		ImGui::RadioButton("Repeat", &App->textures->wrap_mode, 2); ImGui::SameLine();
		ImGui::RadioButton("Mirrored Repeat", &App->textures->wrap_mode, 3);
		ImGui::TreePop();
	}
	ImGui::End();
}


void ModuleRenderExercise::Resize()
{
	frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov * 0.5f) * ((float)App->renderer->width / (float)App->renderer->height));
}
