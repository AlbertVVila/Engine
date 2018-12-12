#include "Application.h"
#include "ModuleRender.h"
#include "ModuleCamera.h"
#include "ComponentCamera.h"
#include "ModuleScene.h"
#include "ModuleProgram.h"
#include "ModuleEditor.h"
#include "ModuleWindow.h"
#include "ModuleTextures.h"
#include "ModuleFileSystem.h"
#include "SDL.h"
#include "GL/glew.h"
#include "imgui.h"
#include "Math/MathFunc.h"
#include "GameObject.h"

ModuleRender::ModuleRender()
{
}

// Destructor
ModuleRender::~ModuleRender()
{
}

// Called before render is available
bool ModuleRender::Init()
{
	LOG("Creating Renderer context");

	InitSDL();

	glewInit();
	InitOpenGL();

	SDL_GL_SetSwapInterval((int)vsync);

	return true;
}

bool ModuleRender::Start()
{
	CreateBlockUniforms(); //TODO: refactor skybox
	std::vector<std::string> faces =
	{
			"right",
			"left",
			"top",
			"bottom",
			"front",
			"back"
	};
	skybox_cubemap = App->textures->LoadCubeMap(faces);
	GenSkyBox();
	return true;
}


update_status ModuleRender::PreUpdate()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	return UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleRender::Update()
{
	glBindFramebuffer(GL_FRAMEBUFFER, App->camera->editorcamera->FBO);
	glClearColor(0.3f, 0.3f, 0.3f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (wireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	SetProjectionUniform(App->camera->editorcamera);
	SetViewUniform(App->camera->editorcamera);
	DrawSkyBox(*App->camera->editorcamera);

	//if (App->scene->maincamera != nullptr) //TODO: refactor frustum + camera
	//{
	App->scene->Draw(App->scene->maincamera->frustum);
	//}

	DrawGizmos();
	
	if (App->scene->maincamera != nullptr)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, App->scene->maincamera->FBO);
		glClearColor(0.3f, 0.3f, 0.3f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		SetProjectionUniform(App->scene->maincamera);
		SetViewUniform(App->scene->maincamera);
		DrawSkyBox(*App->scene->maincamera);

		App->scene->Draw(App->scene->maincamera->frustum);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return UPDATE_CONTINUE;
}

update_status ModuleRender::PostUpdate()
{
	App->editor->RenderGUI();
	SDL_GL_SwapWindow(App->window->window);

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRender::CleanUp()
{
	LOG("Destroying renderer");
	if (UBO != 0)
	{
		glDeleteBuffers(1, &UBO); //TODO: Remember Clean Textures+Buffers + Optimize VAO+VBO creation
	}
	if (skybox_cubemap != 0)
	{
		glDeleteTextures(1, &skybox_cubemap);
	}
	return true;
}

void ModuleRender::OnResize()
{
    glViewport(0, 0, App->window->width, App->window->height);
	App->camera->editorcamera->Resize(App->window->width, App->window->height);
}


void ModuleRender::DrawGizmos() const
{

	glUseProgram(App->program->defaultProgram);
	math::float4x4 model = math::float4x4::identity;
	glUniformMatrix4fv(glGetUniformLocation(App->program->defaultProgram,
		"model"), 1, GL_TRUE, &model[0][0]);

	DrawLines();
	DrawAxis();
	if (App->scene->maincamera != nullptr)//TODO: refactor frustum call
	{
		DrawFrustum();
	}
	glUseProgram(0);
}


void ModuleRender::DrawLines() const
{
	float white[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glUniform4fv(glGetUniformLocation(App->program->defaultProgram,
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

void ModuleRender::DrawAxis() const
{
	glLineWidth(2.0f);

	float red[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	glUniform4fv(glGetUniformLocation(App->program->defaultProgram,
		"Vcolor"), 1, red);

	glBegin(GL_LINES);
	// red X

	glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 0.1f, 0.0f); glVertex3f(1.1f, -0.1f, 0.0f);
	glVertex3f(1.1f, 0.1f, 0.0f); glVertex3f(1.0f, -0.1f, 0.0f);
	glEnd();

	float green[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
	glUniform4fv(glGetUniformLocation(App->program->defaultProgram,
		"Vcolor"), 1, green);

	glBegin(GL_LINES);
	// green Y
	glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
	glVertex3f(0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
	glVertex3f(0.0f, 1.15f, 0.0f); glVertex3f(0.0f, 1.05f, 0.0f);
	glEnd();


	float blue[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
	glUniform4fv(glGetUniformLocation(App->program->defaultProgram,
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

void ModuleRender::DrawFrustum() const
{
	math::Frustum *frustum = &App->scene->maincamera->frustum;
	glUseProgram(App->program->defaultProgram);

	float3 corners[8];
	frustum->GetCornerPoints(corners);

	GLfloat vertices[] = {
		corners[0].x, corners[0].y, corners[0].z, 1.0,
		corners[4].x, corners[4].y, corners[4].z, 1.0,
		corners[6].x, corners[6].y, corners[6].z, 1.0,
		corners[2].x, corners[2].y, corners[2].z, 1.0,
		corners[1].x, corners[1].y, corners[1].z, 1.0,
		corners[5].x, corners[5].y, corners[5].z, 1.0,
		corners[7].x, corners[7].y, corners[7].z, 1.0,
		corners[3].x, corners[3].y, corners[3].z, 1.0,
	};

	GLuint vbo_vertices;
	glGenBuffers(1, &vbo_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLushort elements[] = {
		0, 1, 2, 3,
		4, 5, 6, 7,
		0, 4, 1, 5, 2, 6, 3, 7
	};
	GLuint ibo_elements;
	glGenBuffers(1, &ibo_elements);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	float4x4 model(float4x4::identity);
	glUniformMatrix4fv(glGetUniformLocation(App->program->defaultProgram,
		"model"), 1, GL_TRUE, &model[0][0]);

	float4x4 view(App->scene->maincamera->frustum.ViewMatrix());
	glUniformMatrix4fv(glGetUniformLocation(App->program->defaultProgram,
		"view"), 1, GL_TRUE, &view.Transposed()[0][0]);

	float4x4 proj(App->scene->maincamera->frustum.ProjectionMatrix());
	glUniformMatrix4fv(glGetUniformLocation(App->program->defaultProgram,
		"proj"), 1, GL_TRUE, &proj.Transposed()[0][0]);

	float red[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	glUniform4fv(glGetUniformLocation(App->program->defaultProgram,
		"Vcolor"), 1, red);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,  // attribute
		4,                  // number of elements per vertex, here (x,y,z,w)
		GL_FLOAT,           // the type of each element
		GL_FALSE,           // take our values as-is
		0,                  // no extra data between each position
		0                   // offset of first element
	);

	glLineWidth(4.f);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
	glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0);
	glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid*)(4 * sizeof(GLushort)));
	glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, (GLvoid*)(8 * sizeof(GLushort)));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glLineWidth(1.f);

	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDeleteBuffers(1, &vbo_vertices);
	glDeleteBuffers(1, &ibo_elements);
	glUseProgram(0);
}


void ModuleRender::GenSkyBox()
{
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
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void ModuleRender::DrawSkyBox(const ComponentCamera& camera) const
{
	glDepthMask(GL_FALSE);
	glUseProgram(App->program->skyboxProgram);
	glBindVertexArray(skyboxVAO);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_cubemap);
	float4x4 model(float4x4::FromTRS(camera.frustum.pos,float4x4::identity,float3::one));
	glUniformMatrix4fv(glGetUniformLocation(App->program->skyboxProgram,
		"model"), 1, GL_TRUE, &model[0][0]);

	glUniform1i(glGetUniformLocation(App->program->skyboxProgram, "skybox"), 0);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glUseProgram(0);

	glDepthMask(GL_TRUE);

}


void ModuleRender::InitSDL()
{
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	context = SDL_GL_CreateContext(App->window->window);
	SDL_GetWindowSize(App->window->window, &App->window->width, &App->window->height);
}

void ModuleRender::InitOpenGL() const
{
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_TEXTURE_2D);

	glClearDepth(1.0f);
	glClearColor(0.3f, 0.3f, 0.3f, 1.f);


	glViewport(0, 0, App->window->width, App->window->height);
}

void ModuleRender::CreateBlockUniforms()
{ //TODO: optimize this calls
	unsigned int uniformBlockIndexDefault = glGetUniformBlockIndex(App->program->defaultProgram, "Matrices");
	unsigned int uniformBlockIndexTexture = glGetUniformBlockIndex(App->program->textureProgram, "Matrices");
	unsigned int uniformBlockIndexSkybox = glGetUniformBlockIndex(App->program->skyboxProgram, "Matrices");
	unsigned int uniformBlockIndexFlat = glGetUniformBlockIndex(App->program->flatProgram, "Matrices");
	unsigned int uniformBlockIndexGouraud = glGetUniformBlockIndex(App->program->gouraudProgram, "Matrices");
	unsigned int uniformBlockIndexPhong = glGetUniformBlockIndex(App->program->phongProgram, "Matrices");

	glUniformBlockBinding(App->program->defaultProgram, uniformBlockIndexDefault, 0);
	glUniformBlockBinding(App->program->textureProgram, uniformBlockIndexTexture, 0);
	glUniformBlockBinding(App->program->skyboxProgram, uniformBlockIndexSkybox, 0);
	glUniformBlockBinding(App->program->flatProgram, uniformBlockIndexFlat, 0);
	glUniformBlockBinding(App->program->gouraudProgram, uniformBlockIndexGouraud, 0);
	glUniformBlockBinding(App->program->phongProgram, uniformBlockIndexPhong, 0);

	glGenBuffers(1, &UBO);
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(float4x4), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBufferRange(GL_UNIFORM_BUFFER, 0, UBO, 0, 2 * sizeof(float4x4));
}

void ModuleRender::DrawGUI()
{
	if (ImGui::Checkbox("Depth Test", &depthTest))
	{
		if (App->renderer->depthTest)
		{
			glEnable(GL_DEPTH_TEST);
		}
		else
		{
			glDisable(GL_DEPTH_TEST);
		}
	}

	ImGui::Checkbox("Wireframe", &wireframe);
	if (ImGui::Checkbox("Vsync", &vsync))
	{
		SDL_GL_SetSwapInterval((int)vsync);
	}
}

void ModuleRender::SetViewUniform(ComponentCamera *camera) const
{
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float4x4), sizeof(float4x4), &camera->GetViewMatrix()[0][0]);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void ModuleRender::SetProjectionUniform(ComponentCamera *camera) const
{
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float4x4), &camera->GetProjectionMatrix()[0][0]);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
