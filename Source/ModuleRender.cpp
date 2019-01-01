#include "Application.h"

#include "ModuleRender.h"
#include "ModuleCamera.h"
#include "ModuleScene.h"
#include "ModuleProgram.h"
#include "ModuleEditor.h"
#include "ModuleWindow.h"
#include "ModuleResourceManager.h"

#include "GameObject.h"
#include "ComponentCamera.h"
#include "Skybox.h"

#include "SDL.h"
#include "GL/glew.h"
#include "imgui.h"
#include "Math/MathFunc.h"

ModuleRender::ModuleRender()
{
}

// Destructor
ModuleRender::~ModuleRender()
{
	RELEASE(skybox);
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
	GenBlockUniforms();
	skybox = new Skybox();
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
	return UPDATE_CONTINUE;
}


update_status ModuleRender::PostUpdate()
{
	App->editor->RenderGUI();
	SDL_GL_SwapWindow(App->window->window);

	return UPDATE_CONTINUE;
}

void ModuleRender::Draw(const ComponentCamera& cam, int width, int height) const
{
	glViewport(0, 0, width, height);
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

	SetProjectionUniform(cam);
	SetViewUniform(cam);
	skybox->Draw(*cam.frustum);
	DrawGizmos();
	if (picker_debug)
	{
		for (const auto & line : App->scene->debuglines)
		{
			unsigned shader = App->program->defaultShader->id;
			glUseProgram(shader);
			math::float4x4 model = math::float4x4::identity;
			glUniformMatrix4fv(glGetUniformLocation(shader,
				"model"), 1, GL_TRUE, &model[0][0]);
			glLineWidth(3.0f);
			glBegin(GL_LINES);

			glVertex3f(line.a.x, line.a.y, line.a.z);
			glVertex3f(line.b.x, line.b.y, line.b.z);
			glEnd();
			glUseProgram(0);
		}
	}

	//if (useMainCameraFrustum && App->scene->maincamera != nullptr)
	//{
	//	App->scene->Draw(*cam.frustum);
	//}
	//else
	//{
	App->scene->Draw(*cam.frustum);
	//}


	//if (App->scene->maincamera != nullptr)
	//{
	//	glBindFramebuffer(GL_FRAMEBUFFER, cam.FBO);
	//	glClearColor(0.3f, 0.3f, 0.3f, 1.f);
	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//	SetProjectionUniform(cam);
	//	SetViewUniform(cam);
	//	skybox->Draw(*cam.frustum);
	//	App->scene->Draw(*cam.frustum);
	//}
}
// Called before quitting
bool ModuleRender::CleanUp()
{
	LOG("Destroying renderer");
	if (UBO != 0)
	{
		glDeleteBuffers(1, &UBO);
	}
	skybox->CleanUp();
	return true;
}

void ModuleRender::OnResize()
{
    glViewport(0, 0, App->window->width, App->window->height);
	App->camera->editorcamera->SetAspect((float)App->window->width / (float)App->window->height);
	//App->camera->editorcamera->Resize(App->window->width, App->window->height); //TODO: resize other cameras?
}


void ModuleRender::DrawGizmos() const
{
	unsigned shader = App->program->defaultShader->id;
	glUseProgram(shader);
	math::float4x4 model = math::float4x4::identity;
	glUniformMatrix4fv(glGetUniformLocation(shader,
		"model"), 1, GL_TRUE, &model[0][0]);

	DrawLines();
	DrawAxis();
	if (App->scene->maincamera != nullptr)
	{
		DrawFrustum();
	}
	glUseProgram(0);
}


void ModuleRender::DrawLines() const
{
	float white[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glUniform4fv(glGetUniformLocation(App->program->defaultShader->id,
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

void ModuleRender::DrawAxis() const //TODO: use debug draw
{
	unsigned shader = App->program->defaultShader->id;
	glLineWidth(2.0f);

	float red[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	glUniform4fv(glGetUniformLocation(shader,
		"Vcolor"), 1, red);

	glBegin(GL_LINES);
	// red X

	glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 0.1f, 0.0f); glVertex3f(1.1f, -0.1f, 0.0f);
	glVertex3f(1.1f, 0.1f, 0.0f); glVertex3f(1.0f, -0.1f, 0.0f);
	glEnd();

	float green[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
	glUniform4fv(glGetUniformLocation(shader,
		"Vcolor"), 1, green);

	glBegin(GL_LINES);
	// green Y
	glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
	glVertex3f(0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
	glVertex3f(0.0f, 1.15f, 0.0f); glVertex3f(0.0f, 1.05f, 0.0f);
	glEnd();


	float blue[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
	glUniform4fv(glGetUniformLocation(shader,
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

void ModuleRender::DrawFrustum() const //TODO: Create Separate Frustum class drawer
{
	unsigned shader = App->program->defaultShader->id;
	math::Frustum *frustum = App->scene->maincamera->frustum;
	glUseProgram(shader);

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
	glUniformMatrix4fv(glGetUniformLocation(shader,
		"model"), 1, GL_TRUE, &model[0][0]);

	float4x4 view(frustum->ViewMatrix());
	glUniformMatrix4fv(glGetUniformLocation(shader,
		"view"), 1, GL_TRUE, &view.Transposed()[0][0]);

	float4x4 proj(frustum->ProjectionMatrix());
	glUniformMatrix4fv(glGetUniformLocation(shader,
		"proj"), 1, GL_TRUE, &proj.Transposed()[0][0]);

	float red[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	glUniform4fv(glGetUniformLocation(shader,
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
	glEnable(GL_MULTISAMPLE);

	glClearDepth(1.0f);
	glClearColor(0.3f, 0.3f, 0.3f, 1.f);


	glViewport(0, 0, App->window->width, App->window->height);
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
	ImGui::Checkbox("Main Camera Frustum", &useMainCameraFrustum);
	ImGui::Checkbox("Skybox", &skybox->enabled);
	ImGui::Checkbox("MSAA", &msaa);
	if (msaa)
	{
		ImGui::RadioButton("x2", &msaa_level, 2); ImGui::SameLine();
		ImGui::RadioButton("x4", &msaa_level, 4); ImGui::SameLine();
		ImGui::RadioButton("x8", &msaa_level, 8);
	}
	ImGui::Checkbox("Picker Debug", &picker_debug);
	ImGui::Checkbox("Light Debug", &light_debug);
	
}

void ModuleRender::GenBlockUniforms()
{
	glGenBuffers(1, &UBO); //Block uniform creation
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(float4x4), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBufferRange(GL_UNIFORM_BUFFER, 0, UBO, 0, 2 * sizeof(float4x4));
}

void ModuleRender::AddBlockUniforms(const Shader &shader) const
{ 
	unsigned int uniformBlockIndex = glGetUniformBlockIndex(shader.id, "Matrices");
	glUniformBlockBinding(shader.id, uniformBlockIndex, 0);
}

void ModuleRender::SetViewUniform(const ComponentCamera &camera) const
{
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float4x4), sizeof(float4x4), &camera.GetViewMatrix()[0][0]);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void ModuleRender::SetProjectionUniform(const ComponentCamera &camera) const
{
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float4x4), &camera.GetProjectionMatrix()[0][0]);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
